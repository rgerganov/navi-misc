v 20030525
C 37100 70200 1 0 0 usbheader.sym
{
T 37100 71600 5 10 1 1 0 0
refdes=J1
T 37200 70000 5 10 1 1 0 0
value=USB
}
N 38800 70700 39500 70700 4
N 39500 70700 39500 75200 4
N 39500 75200 38800 75200 4
N 38800 71000 39200 71000 4
N 39200 71000 39200 74800 4
N 39200 74800 38800 74800 4
C 39800 71500 1 0 0 5V-plus-1.sym
C 39900 69800 1 0 0 gnd-1.sym
N 38800 70400 40000 70400 4
N 40000 70400 40000 70100 4
N 38800 71300 40000 71300 4
N 40000 71300 40000 71500 4
C 34300 71500 1 90 0 capacitor-1.sym
{
T 33800 72100 5 10 1 1 180 0
refdes=C3
T 33800 71900 5 10 1 1 180 0
value=0.22uf
}
C 34000 71100 1 0 0 gnd-1.sym
N 34100 71400 34100 71500 4
N 34100 72400 34100 74800 4
N 34100 74800 34600 74800 4
C 36300 72600 1 0 0 resistor-1.sym
{
T 36500 72900 5 10 1 1 0 0
refdes=R1
T 36500 72400 5 10 1 1 0 0
value=1.5k
}
N 36300 72700 34100 72700 4
N 37200 72700 39200 72700 4
C 40900 77400 1 0 0 gnd-1.sym
C 39200 78400 1 0 0 5V-plus-1.sym
C 34200 81900 1 90 0 resistor-1.sym
{
T 33900 82100 5 10 1 1 90 0
refdes=R2
T 34200 81900 5 10 1 1 0 0
value=10k
}
N 34100 81900 34100 81600 4
N 34100 81600 34600 81600 4
C 33900 83000 1 0 0 5V-plus-1.sym
N 34100 83000 34100 82800 4
C 39800 71300 1 270 0 capacitor-2.sym
{
T 40300 70900 5 10 1 1 0 0
refdes=C4
T 40300 70700 5 10 1 1 0 0
value=10uf
}
C 39700 78200 1 0 0 capacitor-1.sym
{
T 40000 78300 5 10 1 1 180 0
refdes=C5
T 40600 78300 5 10 1 1 180 0
value=0.1uf
}
N 38800 78000 41000 78000 4
T 46000 65500 9 20 1 0 0 0
USB Video Switch
T 45900 65100 9 10 1 0 0 0
uvswitch.sch
T 46900 64800 9 10 1 0 0 0
1
T 47400 64800 9 10 1 0 0 0
1
T 50000 65100 9 10 1 0 0 0
$Rev$
T 50000 64800 9 10 1 0 0 0
Micah Dowty
N 38800 78400 39700 78400 4
C 20000 64600 1 0 0 title-bordered-A1.sym
N 40600 78400 41000 78400 4
N 41000 77700 41000 78400 4
C 34500 73500 1 0 0 pic16c765.sym
{
T 38300 82100 5 10 1 1 0 0
refdes=U1
}
C 32850 77800 1 0 0 5V-plus-1.sym
N 33050 77800 33050 77600 4
N 34600 77200 31550 77200 4
C 31800 77400 1 0 0 capacitor-1.sym
{
T 32600 77500 5 10 1 1 180 0
refdes=C9
T 32400 77700 5 10 1 1 0 0
value=0.1uf
}
N 32700 77600 34600 77600 4
N 31800 77600 31550 77600 4
N 31950 76000 31950 76800 4
C 32900 76900 1 180 0 crystal-1.sym
{
T 32600 77100 5 10 1 1 180 0
refdes=X1
T 32200 76500 5 10 1 1 0 0
value=6 MHz
}
N 32900 76800 34600 76800 4
N 34600 76400 31950 76400 4
N 31950 76800 32200 76800 4
C 32150 75100 1 90 0 capacitor-1.sym
{
T 32300 75400 5 10 1 1 180 0
refdes=C11
T 32050 75100 5 10 1 1 0 0
value=20pf
}
C 33350 75100 1 90 0 capacitor-1.sym
{
T 33550 75400 5 10 1 1 180 0
refdes=C12
T 33250 75100 5 10 1 1 0 0
value=20pf
}
N 33150 76000 33150 76800 4
C 31850 74600 1 0 0 gnd-1.sym
C 33050 74600 1 0 0 gnd-1.sym
N 33150 75100 33150 74900 4
N 31950 75100 31950 74900 4
N 31550 77600 31550 76900 4
C 31450 76600 1 0 0 gnd-1.sym
