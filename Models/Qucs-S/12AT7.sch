<Qucs Schematic 0.0.22>
<Properties>
  <View=-47,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=12AT7.dat>
  <DataDisplay=12AT7.dpl>
  <OpenDisplay=1>
  <Script=12AT7.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <Line -30 40 0 -10 #000000 2 1>
  <Line -30 30 60 0 #000000 2 1>
  <Line 30 30 0 10 #000000 2 1>
  <Ellipse -50 -50 100 100 #ff0000 2 1 #c0c0c0 1 0>
  <Line -40 0 20 0 #000000 2 1>
  <Line -10 0 20 0 #000000 2 1>
  <Line 20 0 20 0 #000000 2 1>
  <Line -60 0 20 0 #800000 2 1>
  <Line 0 -35 0 -45 #800000 2 1>
  <Line 30 40 0 30 #800000 2 1>
  <Line -20 -35 40 0 #000000 2 1>
  <.PortSym 30 70 1 90>
  <.PortSym -60 0 2 0>
  <.PortSym 0 -80 3 0>
  <.ID 30 -76 VL>
</Symbol>
<Components>
  <GND * 1 150 220 0 0 0 0>
  <Port P2 1 220 130 4 -52 0 2 "2" 1 "analog" 0 "v" 0 "" 0>
  <Port P1 1 70 190 -23 12 0 0 "1" 1 "analog" 0 "v" 0 "" 0>
  <Port P3 1 70 130 -23 12 0 0 "3" 1 "analog" 0 "v" 0 "" 0>
  <SPICE X1 1 150 160 -26 -91 0 0 "12AT7.cir" 1 "_net1,_net6,_net3" 0 "yes" 0 "none" 0>
</Components>
<Wires>
  <180 130 220 130 "" 0 0 0 "">
  <70 190 120 190 "" 0 0 0 "">
  <70 130 120 130 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
