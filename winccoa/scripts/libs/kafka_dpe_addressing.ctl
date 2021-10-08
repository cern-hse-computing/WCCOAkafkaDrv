/**
 * Library of functions used to address Kafaka Driver DPE
 * @file   kafka_dpe_addressing.ctl
 * @author Adrien Ledeul
 * @date   08/10/2021
 * @modifications:
 *  -[author] [date] [object]
*/

const unsigned fwPeriphAddress_TYPE 			= 1;
const unsigned fwPeriphAddress_DRIVER_NUMBER 	= 2;
const unsigned fwPeriphAddress_REFERENCE		= 3;
const unsigned fwPeriphAddress_DIRECTION		= 4;
const unsigned fwPeriphAddress_DATATYPE			= 5;
const unsigned fwPeriphAddress_ACTIVE 			= 6;
const unsigned kafkaAddress_SUBINDEX = 7;

const unsigned kafkaAddress_MODE_OUT = 1;
const unsigned kafkaAddress_MODE_IN = 2;




/**
 * Called when a DPE connected to this driver is adressed (used to set the proper periph. address config)
 * @param dpe             path to dpe to address (e.g. Consumer.string)
 * @param address         address string (format: CONFIG$DATA_TYPE or TOPIC$KEY$DATA_TYPE or TOPIC$KEY$DEBOUNCING_TIMEFRAME$DATA_TYPE
 * @param driverNum       driver manager number
 * @param mode            adressing mode ( kafkaAddress_MODE_IN or kafkaAddress_MODE_OUT)
 * @param isConfig        is a driver configuration datapoint
 * @return 1 if OK, 0 if not
*/

public int kafkaAddress_addressDPE(string dpe, string address, int driverNum, int mode)
{
  dyn_anytype params;
  dyn_string split;

  try
  {
    split = strsplit(address, "$");
    params[fwPeriphAddress_DRIVER_NUMBER] = driverNum;
    params[fwPeriphAddress_DIRECTION]= mode;
    params[fwPeriphAddress_ACTIVE] = true;
    params[kafkaAddress_SUBINDEX] = 0;

    if(dynlen(split) >= 2)
    {
      int type;
      switch(split[dynlen(split)])
      {
        case "BOOL":
          type = 1000;
          break;
        case "CHAR":
          type = 1001;
          break;
        case "INT32":
          type = 1002;
          break;
        case "INT64":
          type = 1003;
          break;
        case "FLOAT":
          type = 1004;
          break;
        case "STRING":
          type = 1005;
          break;
        case "TIME":
          type = 1006;
          break;
        default:
          DebugN("Error: Unrecognized type \"" + split[dynlen(split)] + "\". DPE " + dpe + " won't be addressed. Full address string: " + address);
          return -1;
      }
      params[fwPeriphAddress_DATATYPE] = type;
    }

    if(dynlen(split) == 2) //config
    {
      params[fwPeriphAddress_REFERENCE] = split[1];
    }
    else if(dynlen(split) == 3) // incoming or outgoing without debouncing timeframe
    {
      // TOPIC$KEY
      params[fwPeriphAddress_REFERENCE] = split[1] + "$" + split[2];
    }
    else if(dynlen(split) == 4) // outgoing
    {
      // TOPIC$KEY$DEBOUNCING_TIMEFRAME
      params[fwPeriphAddress_REFERENCE] = split[1] + "$" + split[2] + "$" + split[3];
    }
    else
    {
        DebugN("Error: Invalid address for DPE " + dpe + " won't be addressed. Full address string: " + address);
    }
    kafkaAddress_setPeriphAddress(dpe, params);
  }
  catch
  {
    DebugN("Error: Uncaught exception in KAFKA_addressDPE: " + getLastException());
    return 0;
  }
  return 1;
}

/**
  * Method setting addressing for KAFKA datapoints elements
  * @param datapoint element for which address will be set
  * @param configuration parameteres
  */
private void kafkaAddress_setPeriphAddress(string dpe, dyn_anytype configParameters){

  int i = 1;
  dyn_string names;
  dyn_anytype values;

	dpSetWait(dpe + ":_distrib.._type", DPCONFIG_DISTRIBUTION_INFO,
						dpe + ":_distrib.._driver", configParameters[fwPeriphAddress_DRIVER_NUMBER] );
	dyn_string errors = getLastError();
  if(dynlen(errors) > 0){
		throwError(errors);
		DebugN("Error: Could not create the distrib config");
		return;
	}

  names[i] = dpe + ":_address.._type";
  values[i++] = DPCONFIG_PERIPH_ADDR_MAIN;
  names[i] = dpe + ":_address.._drv_ident";
  values[i++] = "KAFKA";
  names[i] = dpe + ":_address.._reference";
  values[i++] = configParameters[fwPeriphAddress_REFERENCE];
  names[i] = dpe + ":_address.._mode";
  values[i++] = configParameters[fwPeriphAddress_DIRECTION];
  names[i] = dpe + ":_address.._datatype";
  values[i++] = configParameters[fwPeriphAddress_DATATYPE];
  names[i] = dpe + ":_address.._subindex";
  values[i++] = configParameters[kafkaAddress_SUBINDEX];

  dpSetWait(names, values);
}
