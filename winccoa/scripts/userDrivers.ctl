//////////////////////////////////////////////////////////////////////////////
// If you want to include a new driver
//
// 1. In this script below
//    add new item to dds[i] for names, types and drivers respectively below
//
// 2. In panels/para
//    copy and change address_skeleton.pnl to create a new para-panel for
//    a new driver
//    panel name must be: address_newdrivertype.pnl
//    (in our example below: address_tstdrv1.pnl)
//
//   IMPORTANT: don't change the script in the panel-attributes and the buttons!
//
// 3. In scripts/userPara.ctl
//    add new case selection ( in our example case "tstdrv1":)
//      into the next four functions
//    into scripts/userPara.ctl:
//      upDpGetAddress
//      upDpSetAddress
//      upWritePanelAllAddressAttributes
//      upReadPanelAllAddressAttributes
//    and write the appropriate commands
//
//    global variable used for the _address.. -attributes is
//      anytype dpc;
//        dpc[1]=reference;
//        dpc[2]=subindex;
//        dpc[3]=mode;
//        dpc[4]=start;
//        dpc[5]=interval;
//        dpc[6]=reply;
//        dpc[7]=datatype;
//        dpc[8]=drv_ident;
//        dpc[9]=driver;
//    you dont't have to set all of them, use only the necessary elements!
//////////////////////////////////////////////////////////////////////////////
//
// be careful: always use the same number of driver elements
// (e.g. dyn_strings, cases, etc.)
//
//////////////////////////////////////////////////////////////////////////////
// The examples in this script use a copy of panels/para/address_sim.pnl
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// fill the makeDynString with the new driver datas
//////////////////////////////////////////////////////////////////////////////
dyn_dyn_string main()
{
  dyn_dyn_string dds;

  // names
  // this text will be displayed in the driver type selection combobox in address.pnl
  // Example:
  dds[1]=makeDynString("KAFKA Driver");


  // types
  // this text identifies the driver type. The panel name must have the name
  //   "address_"+typename+".pnl" and must be in panels/para
  // Example:
  dds[2]=makeDynString("kafka");

  // drivers
  // dds[3]=makeDynString();
  // this text will be set in _address.._drv_ident
  // Example:
  dds[3]=makeDynString("KAFKA");

  return (dds);
}
