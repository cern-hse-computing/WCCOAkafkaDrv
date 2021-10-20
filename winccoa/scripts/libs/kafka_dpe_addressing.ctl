/**
 * Library of functions used to address Kafaka Driver DPE
 * @file   kafka_dpe_addressing.ctl
 * @author Adrien Ledeul
 * @date   08/10/2021
 * @modifications:
 *  -[author] [date] [object]
*/



const unsigned kafkaAddress_MODE_OUT = 1;
const unsigned kafkaAddress_MODE_IN = 2;

const unsigned kafkaAddress_DATA_TYPE_BOOL = 1000;
const unsigned kafkaAddress_DATA_TYPE_CHAR = 1001;
const unsigned kafkaAddress_DATA_TYPE_INT32 = 1002;
const unsigned kafkaAddress_DATA_TYPE_INT64 = 1003;
const unsigned kafkaAddress_DATA_TYPE_FLOAT = 1004;
const unsigned kafkaAddress_DATA_TYPE_STRING = 1005;
const unsigned kafkaAddress_DATA_TYPE_TIME = 1006;

private const unsigned kafkaAddress_TYPE 			= 1;
private const unsigned kafkaAddress_DRIVER_NUMBER 	= 2;
private const unsigned kafkaAddress_REFERENCE		= 3;
private const unsigned kafkaAddress_DIRECTION		= 4;
private const unsigned kafkaAddress_DATATYPE			= 5;
private const unsigned kafkaAddress_ACTIVE 			= 6;
private const unsigned kafkaAddress_SUBINDEX = 7;


/**
 * Called when a DPE connected to this driver is adressed (used to set the proper periph. address config)
 * @param dpe             path to dpe to address (e.g. Consumer.valueDpe)
 * @param dataType        Data Type (kafkaAddress_DATA_TYPE_*)
 * @param mode            adressing mode ( kafkaAddress_MODE_IN or kafkaAddress_MODE_OUT)
 * @param driverNum       driver manager number
 * @param topic           Kafka topic
 * @param key             Kafka key
 * @param debouncingMs    Debouncing timeframe, in ms (used for kafkaAddress_MODE_OUT only)
 * @return 1 if OK, 0 if not
*/

public int kafkaAddress_addressDPE(string dpe, unsigned dataType, unsigned mode, unsigned driverNum, string topic, string key, unsigned debouncingMs = 0)
{
  dyn_anytype params;
  try
  {
    params[kafkaAddress_DRIVER_NUMBER] = driverNum;
    params[kafkaAddress_DIRECTION]= mode;
    params[kafkaAddress_ACTIVE] = true;
    params[kafkaAddress_SUBINDEX] = 0;
    params[kafkaAddress_DATATYPE] = dataType;
    params[kafkaAddress_REFERENCE] = topic + "$" + key + (debouncingMs > 0 && mode==kafkaAddress_MODE_OUT ? ("$" + debouncingMs) : "");
    kafkaAddress_setPeriphAddress(dpe, params);
  }
  catch
  {
    DebugN("Error: Uncaught exception in kafkaAddress_addressDPE: " + getLastException());
    return 0;
  }
  return 1;
}

/**
 * Called when a DPE connected to this driver is adressed (used to set the proper periph. address config)
 * @param dpe             path to dpe to address (e.g. Consumer.configDpe)
 * @param dataType        Data Type (kafkaAddress_DATA_TYPE_*)
 * @param mode            adressing mode ( kafkaAddress_MODE_IN or kafkaAddress_MODE_OUT)
 * @param driverNum       driver manager number
 * @param mode            adressing mode ( kafkaAddress_MODE_IN or kafkaAddress_MODE_OUT)
 * @param configName      name of the config (e.g. CONSUMER_STATISTICS)
 * @return 1 if OK, 0 if not
*/

public int kafkaAddress_addressConfigDPE(string dpe, unsigned dataType, unsigned mode, unsigned driverNum, string configName)
{
  dyn_anytype params;
  try
  {
    params[kafkaAddress_DRIVER_NUMBER] = driverNum;
    params[kafkaAddress_DIRECTION]= mode;
    params[kafkaAddress_ACTIVE] = true;
    params[kafkaAddress_SUBINDEX] = 0;
    params[kafkaAddress_DATATYPE] = dataType;
    params[kafkaAddress_REFERENCE] = configName;
    kafkaAddress_setPeriphAddress(dpe, params);
  }
  catch
  {
    DebugN("Error: Uncaught exception in kafkaAddress_addressConfigDPE: " + getLastException());
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
						dpe + ":_distrib.._driver", configParameters[kafkaAddress_DRIVER_NUMBER] );
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
  values[i++] = configParameters[kafkaAddress_REFERENCE];
  names[i] = dpe + ":_address.._mode";
  values[i++] = configParameters[kafkaAddress_DIRECTION];
  names[i] = dpe + ":_address.._datatype";
  values[i++] = configParameters[kafkaAddress_DATATYPE];
  names[i] = dpe + ":_address.._subindex";
  values[i++] = configParameters[kafkaAddress_SUBINDEX];

  dpSetWait(names, values);
}
