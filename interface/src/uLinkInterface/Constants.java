package uLinkInterface;

public class Constants {
	public static enum Command {
		PING(1),
		GET_TIME_FROM_PIC(2),     //'t'
		GET_MEMORY_DATA_FROM_PIC(3),     //'m'
		GET_NODES_CONNECTED_TO_NETWORK(4),      //'c'
		GET_STATE_OF_GRID(5),     //'s'
		PUT_SOLAR_FORECASTS(6),     //'f'
		INIT_SOLAR_FORECASTS(7),      //'F'
		SET_TIME_OF_GRID(8),      //'T'
		RESET_PIC(9),     //'r'
		GET_BATTERY_CHARGE(10),     //'g'
		SET_BATTERY_CHARGE(11),      //'b'
		GET_ENERGY_STATE(12),     //'e'
		SET_DEBUG_MODE(13),
		SYNC_DATA(14),
		SET_VALUE(15),
		GET_VALUE(16),
		GET_EEPROM(17),
		RESET_DATALOGGER(18),
		GET_DATALOGGER(19);
		
		public byte value;
		Command(int value) {
			this.value = (byte)value;
		}
	}
	
	public static  String [] subsystems = {
			"NONE", // this one does not exist, sybsystem IDs begin at 1.
			"MAIN",
			"GRID STATE",
			"SPAM",
			"NETWORK",
			"DATALOGGER",
			"LPS",
			"TOKENPASSING"
	};
}
