import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.net.*;
import java.util.*;
import java.io.*;

public class submit implements ActionListener
{
	public interview head;
	public submit(interview nope)
	{
		head = nope;
	}
	
	public void actionPerformed(ActionEvent e)
	{
		String name, ques, answ;
		StringTokenizer a;
		net.write("submit");
		answ = head.answer.getText();
		a = new StringTokenizer(answ,"\n",false);
		answ = "";
		while(a.hasMoreTokens()) answ = answ + "<br>" + a.nextToken();
		net.write(answ);

		name = net.read();
		ques = net.read();
		head.question.setText(name + " Asks: " + ques);
	}
}