<Qucs Schematic 0.0.22>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=EL34.dat>
  <DataDisplay=EL34.dpl>
  <OpenDisplay=1>
  <Script=EL34.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -30 30 0 -10 #000000 2 1>
  <Line -30 20 60 0 #000000 2 1>
  <Line 45 10 -15 10 #000000 2 1>
  <Line 30 20 0 10 #000000 2 1>
  <.ID 70 34 Tube>
  <Ellipse -50 -60 100 100 #ff0000 2 1 #c0c0c0 1 0>
  <Line -40 10 20 0 #000000 2 1>
  <Line -10 10 20 0 #000000 2 1>
  <Line 20 10 20 0 #000000 2 1>
  <Line -40 10 -20 0 #800000 2 1>
  <.PortSym -60 10 3 0>
  <Line -40 -10 20 0 #000000 2 1>
  <Line -10 -10 20 0 #000000 2 1>
  <Line 20 -10 20 0 #000000 2 1>
  <Line -60 -10 20 0 #800000 2 1>
  <Line -40 -30 20 0 #000000 2 1>
  <Line -10 -30 20 0 #000000 2 1>
  <Line 20 -30 20 0 #000000 2 1>
  <Line 0 -45 0 -45 #800000 2 1>
  <.PortSym 0 -90 1 0>
  <Line 40 -30 5 5 #000000 2 1>
  <Line 45 -25 0 35 #000000 2 1>
  <Line 30 30 0 30 #800000 2 1>
  <.PortSym 30 60 4 0>
  <Line -20 -45 40 0 #000000 2 1>
  <.PortSym -60 -10 2 0>
</Symbol>
<Components>
  <Port P1 1 110 140 -23 12 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P2 1 240 140 4 -52 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <Port P3 1 100 220 -23 12 0 0 "3" 1 "analog" 0 "v" 0 "" 0>
  <Port P4 1 240 220 4 -52 0 2 "4" 1 "analog" 0 "v" 0 "" 0>
  <GND * 1 170 230 0 0 0 0>
  <SPICE X2 1 170 170 -26 -85 0 0 "EL34.cir" 1 "_net1,_net4,_net2,_net3" 0 "yes" 0 "none" 0>
</Components>
<Wires>
  <110 140 140 140 "" 0 0 0 "">
  <100 220 130 220 "" 0 0 0 "">
  <130 200 130 220 "" 0 0 0 "">
  <130 200 140 200 "" 0 0 0 "">
  <200 140 240 140 "" 0 0 0 "">
  <200 200 220 200 "" 0 0 0 "">
  <220 200 220 220 "" 0 0 0 "">
  <220 220 240 220 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
