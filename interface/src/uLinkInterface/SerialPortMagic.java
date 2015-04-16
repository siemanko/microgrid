package uLinkInterface;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import javax.swing.JLabel;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class SerialPortMagic {
	MessageHandler current_handler = null;

	private boolean DEBUG_CHARS = false;
	private boolean DEBUG_COMMANDS = true;
	
	private SerialPort serialPort = null;
	private Queue<int []> commandQueue = null;
	
	List<MessageHandler> handlers;

	Gui gui;
	
	
	
	public void addCommand(int [] command) {
		commandQueue.add(command);
	}
	
	private static SerialPortMagic instance = null;
	
	public static SerialPortMagic get() {
		if (instance == null) {
			instance = new SerialPortMagic();
		}
		return instance;
	}
	
	private SerialPortMagic() {
		
	}
	
	public void init(Gui gui) {
		this.gui = gui;
		
		current_handler = null;
		
		serialPort = new SerialPort(gui.portDefaultName);
		commandQueue = new LinkedList<int []>();
		handlers = new ArrayList<MessageHandler>();
		handlers.add(new PingMessageHandler());
		handlers.add(new StringMessageHandler());
		handlers.add(new StateSyncMessageHandler());
		handlers.add(new GetValueHandler());
		
		startIOLoop();
	}


    
	
	public void uploadForecasts(String file) throws NumberFormatException, IOException{
		Path full_path = Paths.get(System.getProperty("user.dir"), file);
		
		List<Float> nums = new ArrayList<Float>();
		
		for (String line : Files.readAllLines(full_path, StandardCharsets.US_ASCII)) {			
			nums.add(Float.parseFloat(line));		
		}
		int numChunks = 0;
		// 4*chunkSize must be less than 60.
		final int chunkSize = 10;
		for (int chunkStart=0; chunkStart<nums.size(); chunkStart += chunkSize) {
			++numChunks;
			int chunkEnd = Math.min(chunkStart+chunkSize, nums.size());
			int len = 4 + 4 * (chunkEnd-chunkStart);
			int offset = 4*chunkStart;
			int [] buffer = new int[len];
			buffer[0] = Constants.Command.PUT_SOLAR_FORECASTS.value;
			buffer[1] = len;
			byte [] offset_b = Utils.ShortToBytes((short)offset);
			// reverse order.
			buffer[2] = offset_b[1];
			buffer[3] = offset_b[0];
			for (int i = 0; i < (chunkEnd-chunkStart); ++i) {
				byte [] num_b = Utils.FloatToBytes(nums.get(chunkStart+i));
				int num_offset = 4 + 4*i;
				// reverse order
				buffer[num_offset] = num_b[3];
				buffer[num_offset+1] = num_b[2];
				buffer[num_offset+2] = num_b[1];
				buffer[num_offset+3] = num_b[0];
			}
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			addCommand(buffer);
		}
		
		gui.showMessage("SYSTEM: FORECAST UPLOADED. (" + numChunks  + " chunks)");
	}
	
	abstract class MessageHandler {
		public abstract byte typeId();
		// returns true if message ended
		public abstract boolean consumeCharacter(byte chr);
	}

	
	class PingMessageHandler extends MessageHandler {
		public byte typeId() {
			return 1;
		}
		public boolean consumeCharacter(byte chr) {
		
			if (chr == 'C') {
				gui.statusLabel.setText("Status: ONLINE");
				gui.lastPing = System.currentTimeMillis();
			} else {
				gui.statusLabel.setText("Status: OFFLINE");
				return true;
			}
			return true;
		}
	}
	
	class GetValueHandler extends MessageHandler {
		public byte typeId() {
			return Constants.Command.GET_VALUE.value;
		}
		
		SingleValueInterface svi = null;
		
		int buffer_next;
		byte [] buffer;
		public boolean consumeCharacter(byte chr) {
			if (svi == null) {
				svi = SingleValueInterface.getForNumber((int)chr);
				if (svi == null) {
					return true;
				}
				System.out.println("Parsing " + svi.name);
				buffer = new byte[svi.getReponseLength()];
				buffer_next = 0;
				return false;
			} else {
				buffer[buffer_next++] = chr;
				if (buffer_next == buffer.length) {
					svi.parseReponse(buffer);
					svi = null;
					return true;
				} else {
					return false;
				}
			}
		}
	}
	

	class StateSyncMessageHandler extends MessageHandler {
		public byte typeId() {
			return 3;
		}
		
		StateParser parser = null;
		
		public boolean consumeCharacter(byte chr) {
			if (parser == null) {
				parser = StateParser.get_parser_for_number((int)chr);
				return false;
			} else {
				if (parser.consumeCharacter(chr)) {
					parser.updateGui(gui);
					parser = null;
					return true;
				} else {
					return false;
				}
			}
		}
	}
	
	
	class StringMessageHandler extends MessageHandler {

		public byte typeId() {
			return 2;
		}
		
		String out = null;
		
		byte [] special_buffer = new byte[10];
		int special_buffer_next = 0;
		// MODES
		// 0 - char mode
		// 1 - float mode
		// 2 - int mode
		int mode = 0; 
		// if in int or float mode shows num chars remaining.
		
		public boolean consumeCharacter(byte chr) {
			if (out == null) {
				if (0 <= chr && chr < Constants.subsystems.length) {
					out = "" + Constants.subsystems[chr] + ": ";
					return false;
				} else {
					return true;
				}
			} else {
				if (mode == 0) {
					if (chr == 0) { // in char mode and received termination char.
						//out = out + "\n";
						// System.out.println(out.charAt(5+ 13));
						// System.out.println((byte)out.charAt(5+13));
						if (gui.output_enabled.isSelected())
							gui.debug_text.setText(gui.debug_text.getText()+out +"\n");
						out = null;
						return true;
					} else {
						out = out + (char)chr;
						
						if (out.substring(out.length()-2, out.length()).equals("%f") ) {
							out = out.substring(0, out.length()-2);
							mode = 1;
							special_buffer_next = 0;
						} else if (out.substring(out.length()-2, out.length()).equals("%d") ) {
							out = out.substring(0, out.length()-2);
							mode = 2;
							special_buffer_next = 0;
						}
						return false;
					}
				} else if (mode == 1) { // float
					special_buffer[special_buffer_next++] = chr;
					if (special_buffer_next == 4) {
						out = out + Utils.BytesToFloat(special_buffer);
						mode = 0;
					}
					return false;
				} else if (mode == 2) { // int
					special_buffer[special_buffer_next++] = chr;
					if (special_buffer_next == 2) {
						out = out + Utils.BytesToShort(special_buffer).toString();
						mode = 0;
					}			
					return false;
				}
						
						
				return true;
			}
		}
	}
	
	public synchronized void resetConnection() {
		gui.statusLabel.setText("Status: OFFLINE");
		boolean success = false;
		while(!success) {
			try {
				if (serialPort.isOpened()) {
					serialPort.closePort();
				}
				current_handler = null;
				serialPort.openPort();
				
			    serialPort.setParams(SerialPort.BAUDRATE_57600, 
				                     SerialPort.DATABITS_8,
				                     SerialPort.STOPBITS_1,
				                     SerialPort.PARITY_NONE);
				success = true;
			}
		 	catch (SerialPortException ex) {
		        System.out.println(ex);
		 	}
			if (!success) {
				System.out.println("Trouble connecting");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
				}
			}
		}
	}
	
	public void ensureConnection() {
		if (!serialPort.isOpened()) {
			resetConnection();
		}
	}
	
	private void startIOLoop() {
		Thread t = new Thread(new Runnable() {
			@Override
			public void run() {
				while(true) {
					ensureConnection();
					try {
						ensureConnection();
						byte [] buffer = serialPort.readBytes(1, 200);
						byte read = buffer[0];
						if (DEBUG_CHARS) {
							System.out.println(read);
						}
						if (current_handler == null) {
							// System.out.println(read);
							for (MessageHandler h: handlers) {
								if (read == h.typeId()) {
									current_handler = h;
								}
							}
							if (current_handler == null) {
								System.out.println("Unknown message: " + read);
							}
						} else {
							if(current_handler.consumeCharacter(read)) {
								current_handler = null;
							}
						}
					} catch (SerialPortException e) {
						resetConnection();
					} catch (SerialPortTimeoutException e) {
					}
					
					ensureConnection();
					while (!commandQueue.isEmpty()) {
						
						try {
							Thread.sleep(50);
						} catch (InterruptedException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						}
						
						int command [] = commandQueue.poll();
						if (DEBUG_COMMANDS) {
							String str = "COMMAND:";
							for (int i=0; i<command.length; ++i) {
								str = str + " " + command[i];
							}
							System.out.println(str);
						}
						boolean success = false;
						try {
							success = serialPort.writeIntArray(command);
						} catch (SerialPortException e) {
						}
						if (!success) {
							resetConnection();
						}
					}
				}
			}
		});
		t.start();
	}
}
