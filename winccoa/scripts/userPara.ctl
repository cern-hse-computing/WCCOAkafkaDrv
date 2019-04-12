//////////////////////////////////////////////////////////////////////////////
// If you want to include a new driver:
//
// 1. In scripts/userDrivers.ctl
//    add new item to dds[i] for names, types and drivers respectively below
//
// 2. In panels/para
//    copy and change address_skeleton.pnl to create a new para-panel for
//    a new driver
//    panel name must be: address_newdrivertype.pnl
//    (in our example below: address_tstdrv1.pnl)
//
//    IMPORTANT: don't change the script in the panel-attributes and the buttons!
//
// 3. In this script below
//    add new case selection ( in our example case "tstdrv1":)
//     into the next four functions
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
//    you don't have to set all of them, use only the necessary elements!
//////////////////////////////////////////////////////////////////////////////
//
// be careful: always use the same number of driver elements
// (e.g. dyn_strings, cases, etc.)
//
//////////////////////////////////////////////////////////////////////////////
// The examples in this script use a copy of panels/para/address_sim.pnl
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// main calls the needed function
// fct = 1: upDpGetAddress
// fct = 2: upDpSetAddress
// fct = 3: upWritePanelAllAddressAttributes
// fct = 4: upReadPanelAllAddressAttributes
//////////////////////////////////////////////////////////////////////////////
anytype main(string dpe, int Id, anytype dpc, int fct)
{
  bool ok, all_right;

  switch (fct)
  {
    case 1: upDpGetAddress(dpe, Id, dpc);
      break;
    case 2: upDpSetAddress(dpe, Id, dpc, all_right); dpc[99]=all_right;
      break;
    case 3: upWritePanelAllAddressAttributes(dpe, Id, dpc);
      break;
    case 4: upReadPanelAllAddressAttributes(dpe, Id, dpc, all_right); dpc[99]=all_right;
      break;
  }
  return (dpc);
}

//////////////////////////////////////////////////////////////////////////////
// this function reads the datapoint values
//////////////////////////////////////////////////////////////////////////////
upDpGetAddress(string dpe, int Id, anytype &dpc)
{
  int         datatype,driver,i,distrib_type;
  bool        all_right,active;
  char        mode;
  time        start,interval,reply;
  string      drv_ident,reference,config=paGetDpConfig(globalOpenConfig[Id]),
              dpn=dpSubStr(dpe,DPSUB_DP)+".",pg,ser_nr;
  unsigned    subindex,offset;

  switch (globalAddressOld[Id])
  {
    case "kafka":
      dpGet(dpe+":"+config+".._active",active,
	    dpe+":"+config+".._reference",reference,
	    dpe+":"+config+".._subindex",subindex,
	    dpe+":"+config+".._mode",mode,
	    dpe+":"+config+".._reply",reply,
	    dpe+":"+config+".._datatype",datatype,
	    //dpe+":"+config+".._poll_group",pg,
	    dpe+":"+config+".._drv_ident",drv_ident,
	    dpe+":_distrib.._driver",driver);
      dpc[1]=reference;
      dpc[2]=subindex;
      dpc[3]=mode;
      dpc[6]=reply;
      dpc[7]=datatype;
      dpc[8]=drv_ident;
      if (driver<1) driver=1;
      dpc[9]=driver;
      dpc[11]=pg;
      dpc[12]=active;
      break;
    default: break;
  }
}

//////////////////////////////////////////////////////////////////////////////
upDpSetAddress(string dpe, int Id, dyn_anytype dpc, bool &all_right)
{
  bool       ok;
  string     config=paGetDpConfig(globalOpenConfig[Id]),dpn=dpSubStr(dpe,DPSUB_DP)+".";
  dyn_int    drivers;
  dyn_string sPara;

  switch (globalAddressOld[Id])
  {
  case "kafka":
    paErrorHandlingDpSet(
			 dpSetWait(dpe+":_distrib.._driver",dpc[9]),all_right);
    paErrorHandlingDpSet(
			 dpSetWait(dpe+":_address.._type", DPCONFIG_PERIPH_ADDR_MAIN,
				   dpe+":"+config+".._reference",dpc[1],
				   dpe+":"+config+".._subindex",dpc[2],
				   dpe+":"+config+".._mode",dpc[3],
				   dpe+":"+config+".._reply",dpc[6],
				   dpe+":"+config+".._datatype",dpc[7],
           //dpe+":"+config+".._poll_group", dpc[11],
				   dpe+":"+config+".._drv_ident",dpc[8]),all_right);
    paErrorHandlingDpSet(
			 dpSetWait(dpe+":_address.._active",dpc[12]),all_right);
    break;
    default: break;
  }
}

//////////////////////////////////////////////////////////////////////////////
upWritePanelAllAddressAttributes(string dpe, int Id, anytype dpc)
{
  int        i,j=0,pos=1,trafoPos,
             lowlevel,q,intern,
             plc=0,comp=0,rack=0,slot=0,mode=0,
             cbase=0,cfactor=1,ctime,
             ibase=0,ifactor=0,icommand,
             smooth=0,flutter=1,
             art, df=0, m, m13;
  bool       err=false;
  float      itime;
  string     s,text,reference, pg;
  dyn_int    typ;
  dyn_string ds,dss;

  switch (globalAddressOld[Id])
  {
  case "kafka":
    if (dpc[3]>=64) { dpc[3]-=64; lowlevel=1;}
    if (dpc[3]>=32) { dpc[3]-=32;}
    if (dpc[3]<=0) dpc[3]=1;
    if (dpc[3]==1 ||dpc[3]==5)
    {
      setMultiValue(//"lowlevel","visible",false,
                    "lowlevel", "state", 0,lowlevel);
      setValue("einaus","number",0);
    }
    else if (dpc[3] >= 6)
    {
      setMultiValue("einaus","number",2,
                    //"lowlevel","visible",true,
                    "lowlevel", "state", 0,lowlevel);
    }
    else
    {
      setMultiValue("einaus","number",1,
                    //"lowlevel","visible",true,
                    "lowlevel", "state", 0,lowlevel);
    }

    if (dpc[3] == 2 || dpc[3] == 6)
      setMultiValue("inputmode","number",0);
    else if (dpc[3] == 4 || dpc[3] == 7)
      setMultiValue("inputmode","number",1);
    else if (dpc[3] == 3 || dpc[3] == 8)
      setMultiValue("inputmode","number",2);
    //pg = dpc[11];
    //pg = strltrim(pg, "_");
    setMultiValue("cboAddressActive","state",0,dpc[12],
                  "reference","text",dpc[1],
                  "subindex","text",dpc[2],
                  //"pollgroup","text",pg,
                  "trans_art","selectedPos",dpc[7]-999,
                  "Treiber","text",dpc[9]);
    break;
  default:
    break;
  }
}

//////////////////////////////////////////////////////////////////////////////
upReadPanelAllAddressAttributes(string dpe, int Id, dyn_anytype &dpc, bool &readOK)
{
  int        pos,i,j,k,l,mode,n,o,p,driver,datatype,
             lowlevel,q;
  bool       active;
  string     s,text,pg;

  readOK=true;
  switch (globalAddressOld[Id])
  {
  case "kafka":
    getMultiValue("cboAddressActive","state",0,active,
                  "trans_art","selectedPos",j,
                  "Treiber","text",driver,
                  "subindex","text",l,
                  "reference","text",s,
                  "einaus","number",p,
                  "inputmode","number",q,
                  //"pollgroup","text",pg,
                  "lowlevel","state",0,lowlevel);
    // transformation
    j = j+999;

    // mode
    if (p == 0)
      //Output
      mode = 1;
    else if (p == 1)
    {
      // Input
      if (q == 0)
        mode = 2;
      else if (q == 1)
        mode = 4;
      else
        mode = 3;
    }
    else
    {
      // In/Output
      if (q == 0)
        mode = 6;
      else if (q == 1)
        mode = 7;
      else
        mode = 8;
    }
    if (lowlevel) mode+=64;

    // fill the dyn_anytype
    dpc[1]=s;
    dpc[2]=l;
    dpc[3]=mode;
    dpc[7]=j;
    dpc[8]=globalAddressDrivers[dynContains(globalAddressTypes,globalAddressNew[paMyModuleId()])];
    dpc[9]=driver;
    dpc[11]= ""; //polling group
    dpc[12]=active;
    break;
    default: break;
  }
}
