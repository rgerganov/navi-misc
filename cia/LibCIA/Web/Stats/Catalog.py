""" LibCIA.Web.Stats.Catalog

Implements the 'catalog' section in stats, pages, a fast way to
get an overview of all stats targets directly under the current one.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from __future__ import division
from twisted.internet import defer
from LibCIA.Web import Template
from LibCIA import Database, Stats, TimeUtil
import Nouvelle, time, math
import Link


class TargetTitleColumn(Nouvelle.Column):
    """A Column displaying a target's title as a StatsLink. To avoid having to make
       a query for every row in the table, this looks for the title to use in the
       indicated SQL query column number.
       """
    heading = 'title'
    def __init__(self, pathIndex, titleIndex):
        self.pathIndex = pathIndex
        self.titleIndex = titleIndex

    def _findTitle(self, row):
        """Use our titleIndex to return the title according to the metadata if that
           exists, otherwise make up a title based on the path.
           """
        title = row[self.titleIndex]
        if title is None:
            return Stats.Target.StatsTarget(row[self.pathIndex]).name
        else:
            return title
        # Note that we don't have to worry about the case (name is None)
        # because this is only used for listing children of some target,
        # and the root target has no parent.

    def getValue(self, row):
        # For case-insensitive sorting by title
        return self._findTitle(row).lower()

    def render_data(self, context, row):
        # Create a StatsLink. Note that we could leave it up to the StatsLink
        # to look up the title, but that would end up creating an SQL query
        # for each row in the table- not good when we're trying to view a page
        # with 1000 projects without making our server explode.
        target = Stats.Target.StatsTarget(row[self.pathIndex])
        return Link.StatsLink(target, text=self._findTitle(row))


class IndexedUnitColumn(Nouvelle.IndexedColumn):
    """A Nouvelle.IndexedColumn that appends a fixed unit qualifier to each rendered item.
       Hides individual cells when zero, hides the column when all cells are zero.
       """
    def __init__(self, heading, index, singularUnit='item', pluralUnit='items'):
        self.singularUnit = singularUnit
        self.pluralUnit = pluralUnit
        Nouvelle.IndexedColumn.__init__(self, heading, index)

    def isVisible(self, context):
        return context['table'].reduceColumn(self, self._visibilityTest)

    def _visibilityTest(self, seq):
        """Visibility testing operator for this column's data"""
        for item in seq:
            if item != 0:
                return True
        return False

    def render_data(self, context, row):
        value = row[self.index]
        if value == 0:
            return ''
        elif value == 1:
            unit = self.singularUnit
        else:
            unit = self.pluralUnit
        return "%s %s" % (row[self.index], unit)


class RankIndexedColumn(Nouvelle.IndexedColumn):
    """An IndexedColumn tweaked for numeric rankings.
       None is treated as zero, sorting is reversed, the column
       is hidden if all values are zero or less.
       """
    def cmp(self, a, b):
        """Reverse sort"""
        return -cmp(a[self.index],b[self.index])

    def getValue(self, row):
        return row[self.index] or 0

    def isVisible(self, context):
        return context['table'].reduceColumn(self, self._visibilityTest)

    def _visibilityTest(self, seq):
        """Visibility testing operator for this column's data"""
        for item in seq:
            if item != 0:
                return True
        return False


class IndexedBargraphColumn(RankIndexedColumn):
    """An IndexedColumn that renders as a logarithmic bar chart"""
    def render_data(self, context, row):
        value = row[self.index]
        if not value:
            return ''
        logMax = math.log(context['table'].reduceColumn(self, max))
        if logMax > 0:
            fraction = math.log(value) / logMax
        else:
            fraction = 1
        return Template.Bargraph(fraction)[ value ]


class IndexedPercentColumn(RankIndexedColumn):
    """An IndexedColumn that renders itself as a percent of the column's total"""
    def render_data(self, context, row):
        value = row[self.index]
        if not value:
            return ''
        total = context['table'].reduceColumn(self, sum)
        return "%.03f" % (value / total * 100)


class TargetLastEventColumn(Nouvelle.IndexedColumn):
    """A Column displaying the amount of time since the last message was delivered to
       each target, given a column containing the last event timestamp.
       """
    def getValue(self, row):
        """Returns the number of seconds since the last event"""
        lastTime = row[self.index]
        if lastTime:
            return time.time() - lastTime

    def isVisible(self, context):
        return context['table'].reduceColumn(self, self._visibilityTest)

    def _visibilityTest(self, seq):
        """Visibility testing operator for this column's data"""
        for item in seq:
            if item:
                return True
        return False

    def render_data(self, context, target):
        value = self.getValue(target)
        if value is None:
            return ''
        else:
            return "%s ago" % TimeUtil.formatDuration(self.getValue(target))


class CatalogSection(Template.Section):
    """A Section displaying links to all children of a StatsTarget, with
       other information about the children displayed as applicable.
       """
    title = "catalog"

    query = """
    SELECT
        T.target_path,
        M_TITLE.value,
        C_TODAY.event_count,
        C_YESTERDAY.event_count,
        C_FOREVER.event_count,
        C_FOREVER.last_time,
        COUNT(CHILD.target_path)
    FROM stats_catalog T
        LEFT OUTER JOIN stats_catalog  CHILD       ON (CHILD.parent_path = T.target_path)
        LEFT OUTER JOIN stats_metadata M_TITLE     ON (T.target_path = M_TITLE.target_path     AND M_TITLE.name     = 'title')
        LEFT OUTER JOIN stats_counters C_TODAY     ON (T.target_path = C_TODAY.target_path     AND C_TODAY.name     = 'today')
        LEFT OUTER JOIN stats_counters C_YESTERDAY ON (T.target_path = C_YESTERDAY.target_path AND C_YESTERDAY.name = 'yesterday')
        LEFT OUTER JOIN stats_counters C_FOREVER   ON (T.target_path = C_FOREVER.target_path   AND C_FOREVER.name   = 'forever')
        WHERE T.parent_path = %(path)s
    GROUP BY
        T.target_path,
        M_TITLE.value,
        C_TODAY.event_count,
        C_YESTERDAY.event_count,
        C_FOREVER.event_count,
        C_FOREVER.last_time
    """

    columns = [
        TargetTitleColumn(pathIndex=0, titleIndex=1),
        IndexedBargraphColumn('events today', 2),
        IndexedBargraphColumn('events yesterday', 3),
        IndexedBargraphColumn('total events', 4),
        IndexedPercentColumn('% total', 4),
        TargetLastEventColumn('last event', 5),
        IndexedUnitColumn('contents', 6),
        ]

    def __init__(self, target):
        self.target = target

    def render_rows(self, context):
        # First we run a big SQL query to gather all the data for this catalog.
        # Control is passed to _render_rows once we have the query results.
        result = defer.Deferred()
        Database.pool.runQuery(self.query % {
            'path': Database.quote(self.target.path, 'varchar'),
            }).addCallback(
            self._render_rows, context, result
            ).addErrback(result.errback)
        return result

    def _render_rows(self, queryResults, context, result):
        if queryResults:
            result.callback([Template.Table(list(queryResults), self.columns, id='catalog')])
        else:
            result.callback(None)


### The End ###
