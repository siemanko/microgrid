package uLinkInterface;

import javax.swing.JPanel;

abstract public class StateParser {

	public static class DebugModeStateParser extends StateParser {
		Integer totalModes = null;
		Integer nextMode = null;
		byte [] modeStates = null;
		
		boolean consumeCharacter(byte chr) {
			if (totalModes == null) {
				totalModes = (int)chr;
				System.out.println(totalModes);

				nextMode = 0;
				if (totalModes > 0) {
					modeStates = new byte[totalModes];
					return false;
				} else {
					return true;
				}
			} else {
				if (nextMode < modeStates.length) {
					modeStates[nextMode++] = (byte)chr;
					return nextMode == totalModes;
				} else {
					return true;
				}
			}
		}

		void updateGui(Gui gui) {
			for (int i=0; i < Math.min(totalModes, gui.debug_modes.length); ++i) {
				gui.debug_modes[i].setSelected(modeStates[i] > 0);
			}
		}		
	}
	
	public static StateParser get_parser_for_number(int num) {
		if (num == 1) return new DebugModeStateParser();
		return null;
	}
	
	abstract boolean consumeCharacter(byte chr);
	abstract void updateGui(Gui gui);
	
}
