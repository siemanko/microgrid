package uLinkInterface;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.Timer;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.text.DefaultCaret;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class Gui extends JPanel {
	public final String portDefaultName = "COM17";
		
	public final JLabel statusLabel;
	public static void main(String args[]) {
		new Gui();
	}
	
	public final JTextArea debug_text;
	public JCheckBox[] debug_modes;

	private SerialPortMagic serial;

	public JCheckBox output_enabled;
	
	private JButton simpleCommandButton(String label, final int command) {
		JButton btn = new JButton(label);
		btn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				serial.addCommand(new int [] {command, 3, 0});
			}
		});
		return btn;
	}

	public void showMessage(String msg) {
		if (output_enabled.isSelected()) {
			debug_text.setText(debug_text.getText() + msg + "\n");
		}
	}
	
	private JPanel buildForecast() {
		JPanel forecast = new JPanel();
		forecast.setLayout(new BoxLayout(forecast, BoxLayout.X_AXIS));
		final JTextField forecastFile = new JTextField("SunnyDay.txt");
		forecastFile.setMaximumSize(new Dimension(100, 20));
		forecast.add(forecastFile);
		JButton forecastButton = new JButton("Send Forecast");
		forecastButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					serial.uploadForecasts(forecastFile.getText());
				} catch (NumberFormatException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}
		});
		forecast.add(forecastButton);
		return forecast;
	}
	
	private JButton EEPROMButton(String label, final JTextField addr, final int code) {
		JButton i = new JButton(label);
		i.setMaximumSize(new Dimension(60, 20));
		i.setFont(new Font(Font.SANS_SERIF, Font.PLAIN, 9));
		
		i.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				int [] command = new int[5];
				command[0] = Constants.Command.GET_EEPROM.value;
				command[1] = 5;
				command[2] = code;
				byte [] addr_raw = Utils.ShortToBytes(Short.parseShort(addr.getText()));
				// reverse order
				command[3] = addr_raw[1];
				command[4] = addr_raw[0];
				serial.addCommand(command);
			}
		});
		return i;
	}
	
	private JPanel buildEEPROMPanel() {
		JPanel eeprom = new JPanel();
		eeprom.setLayout(new BoxLayout(eeprom, BoxLayout.X_AXIS));
		eeprom.add(new JLabel("Addr"));
		final JTextField addr = new JTextField();
		eeprom.add(addr);
		
		addr.setMaximumSize(new Dimension(60, 20));
		
		
		
		eeprom.add(EEPROMButton("char", addr, 0));
		eeprom.add(EEPROMButton("int", addr, 1));
		eeprom.add(EEPROMButton("float", addr, 2));
		return eeprom;
	}
	
	public Gui() {
		statusLabel = new JLabel("Status: OFFLINE");

		
		debug_text = new JTextArea(5, 20);
		JScrollPane debug_pane = new JScrollPane(debug_text);
		debug_pane.setVerticalScrollBarPolicy(
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		debug_pane.setPreferredSize(new Dimension(500, 700));
		debug_pane.setAutoscrolls(true);

		debug_text.setEditable(false);
		debug_text.setWrapStyleWord(true);
		DefaultCaret caret = (DefaultCaret)debug_text.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
		
		JPanel settings = new JPanel();
		
		debug_modes = new JCheckBox [Constants.subsystems.length - 1];
		for (int i = 0; i < debug_modes.length; ++i) {
			final int index = i;
			debug_modes[i] = new JCheckBox(Constants.subsystems[i+1]);
			debug_modes[i].addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent arg0) {
					int[] command = new int[] {Constants.Command.SET_DEBUG_MODE.value, 4, (byte)(index+1), debug_modes[index].isSelected() ? 1: 0};
					serial.addCommand(command);					
				}
			});
		}
		

		
		JButton synchData = new JButton("SYNC_DATA");
		synchData.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				serial.addCommand(new int [] {Constants.Command.SYNC_DATA.value, 3, 0});
				for (SingleValueInterface svi : SingleValueInterface.values()) {
					try {
						Thread.sleep(5);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					svi.requestValue();
				}
			}
		});
		
		

		
		JButton clearOutput = new JButton("CLEAR OUTPUT");
		clearOutput.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				debug_text.setText("");
			}
		});
		
		JButton unjam = new JButton("UNJAM (many required)");
		unjam.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				int [] command = new int [60];
				for (int i=0; i<60; ++i) command[i] = 69;
				command[59] = 70;
				serial.addCommand(command);
				serial.current_handler = null;
			}
		});
		
		
		setLayout(new BorderLayout());
		// ADDING TO JPANEL
		
		ArrayList<JComponent> settings_components = new ArrayList<JComponent>();
		
		settings.setPreferredSize(new Dimension(300,690));
		settings.setBorder(new EmptyBorder(10, 10, 10, 10));
		settings.setLayout(new BoxLayout(settings, BoxLayout.Y_AXIS));
	
		
		output_enabled = new JCheckBox("Output enabled");
		output_enabled.setSelected(true);
		settings_components.add(output_enabled);
		
		
		settings_components.add(statusLabel);
		
		pinging = new JCheckBox("Pinging enabled");
		pinging.setSelected(false);
		settings_components.add(pinging);
		
		settings_components.add(new JLabel("Debug enables: "));
		for (JCheckBox box : debug_modes) {
			settings_components.add(box);
		}
		
		for(SingleValueInterface svi: SingleValueInterface.values()) {
			settings_components.add(svi.getGui(this));

		}
		settings_components.add(new JLabel("" + (int)'A' + " is A, " + (int)'B' + " is B"));
		
		settings_components.add(buildForecast());
		settings_components.add(buildEEPROMPanel());
		settings_components.add(synchData);
		settings_components.add(simpleCommandButton("RESET PIC", Constants.Command.RESET_PIC.value));
		settings_components.add(simpleCommandButton("GRID STATE", Constants.Command.GET_STATE_OF_GRID.value));
		settings_components.add(simpleCommandButton("CONNECTED NODES", Constants.Command.GET_NODES_CONNECTED_TO_NETWORK.value));
		settings_components.add(simpleCommandButton("ENERGY STATE", Constants.Command.GET_ENERGY_STATE.value));
		settings_components.add(simpleCommandButton("RESET DATALOGGER", Constants.Command.RESET_DATALOGGER.value));
		settings_components.add(unjam);
		settings_components.add(clearOutput);
		
		
		for (JComponent c: settings_components) {
			c.setAlignmentX(LEFT_ALIGNMENT);
			settings.add(c);
		}

		add(settings, BorderLayout.LINE_START);
		add(debug_pane, BorderLayout.LINE_END);

		this.setSize(480, 690);
		// pack();
		setVisible(true);

		
		JFrame frame = new JFrame("uL " + portDefaultName);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		//Add contents to the window.
		frame.add(this);
		
		//Display the window.
		frame.pack();
		frame.setVisible(true);
		
		serial = SerialPortMagic.get();
		serial.init(this);
		
		initActions();

	}
	
	private JCheckBox pinging = null;
	
	long lastPing = 0;
	
	long getLastPing() {
		return lastPing;
	}
	
	public void initActions() {
        int delay = 1000; //milliseconds
        final Gui self = this;
		ActionListener taskPerformer = new ActionListener() {
		    public void actionPerformed(ActionEvent evt) {
		    	if (pinging.isSelected()) {
		    		serial.addCommand(new int []{Constants.Command.PING.value,3,0});
		    
			    	if (System.currentTimeMillis() - self.getLastPing() > 3000) {
			    		statusLabel.setText("Status: OFFLINE");
			    	}
		    	} else {
		    		statusLabel.setText("Status: PINGING DISABLED");
		    	}
		        /*if(uLinkInterface.isConnected()) {
		        	statusLabel.setText("Status: ONLINE");
		        } else {
		        	statusLabel.setText("Status: OFFLINE");
		        }*/
		    }			
		};
		new Timer(delay, taskPerformer).start();
	}
}
