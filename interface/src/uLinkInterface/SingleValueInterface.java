package uLinkInterface;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Arrays;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;

public enum SingleValueInterface {
	DAYS("days", 6, "int"),
	HOURS("hours", 1, "int"),
	MINUTES("minutes", 2, "int"),
	SECONDS("seconds", 3, "int"),
	BATTERY("battery", 4, "float"),
	UID("UID", 5, "int"),
	DEVICE_TYPE("DEVICE_TYPE", 7, "int"),
	BALANCE("Balance", 8, "int");
	
	public int getReponseLength() {
		if (this.type == "int") return 2;
		else if (this.type == "float") return 4;
		Utils.stopAndEducate("getReponseLength not implemented for type " + type);
		return 0;
	}
	
	public void parseReponse(byte [] resp) {
		if (type == "int") {
			// remove %d
			System.out.println("Bytes " + resp[0] + " " + resp[1]);
			Short s = Utils.BytesToShort(resp);
			this.textArea.setText("" + s);
		} else if (type == "float") {
			Float f = Utils.BytesToFloat(resp);
			this.textArea.setText("" + f);
		} else {
			Utils.stopAndEducate("parseResponse not implemented for type " + type);
		}
	}
	
	public final String name;
	public final int number;
	public final String type;
	private JTextArea textArea;
	
	private SerialPortMagic serial;
	
	public static SingleValueInterface getForNumber(int number) {
		for (SingleValueInterface svi: SingleValueInterface.values()) {
			if (svi.number == number) return svi;
		}
		return null;
	}
	
	SingleValueInterface(String name, int number, String type) {
		this.name = name;
		this.number = number;
		this.type = type;
		if (serial == null) {
			serial = SerialPortMagic.get();
		}
	}
	
	void requestValue() {
		serial.addCommand(new int[] {Constants.Command.GET_VALUE.value, 3, this.number} );
	}
	
	void requestValueChange() {
		int [] command = null;
		if (type == "int") {
			Short val = Short.parseShort(textArea.getText());
			byte [] val_repr = Utils.ShortToBytes(val);
			command = new int [5];
			command[0] = Constants.Command.SET_VALUE.value;
			command[1] = 5;
			command[2] = this.number;
			// inverted order
			command[3] = val_repr[1];
			command[4] = val_repr[0];
			System.out.println("" + val_repr[1] + " " + val_repr[0]);
		} else if (type == "float") {
			Float val = Float.parseFloat(textArea.getText());
			byte [] val_repr = Utils.FloatToBytes(val);
			command = new int [7];
			command[0] = Constants.Command.SET_VALUE.value;
			command[1] = 7;
			command[2] = this.number;
			// inverted order
			command[3] = val_repr[3];
			command[4] = val_repr[2];
			command[5] = val_repr[1];
			command[6] = val_repr[0];
		}
		serial.addCommand(command);
	}
	
	public JPanel getGui(Gui parent) {
		JPanel mypanel = new JPanel();
		mypanel.setMinimumSize(new Dimension(300, 30));
		mypanel.setMaximumSize(new Dimension(300, 30));

		mypanel.setLayout(new BoxLayout(mypanel, BoxLayout.X_AXIS));
		
		JLabel label = new JLabel(this.name);
		textArea = new JTextArea("0");
		textArea.setMaximumSize(new Dimension(80, 20));
		JButton setButton = new JButton("SET");
		setButton.setMaximumSize(new Dimension(60, 20));
		setButton.setFont(new Font(Font.SANS_SERIF, Font.PLAIN, 9));
		JButton getButton = new JButton("GET");
		getButton.setFont(new Font(Font.SANS_SERIF, Font.PLAIN, 9));
		getButton.setMaximumSize(new Dimension(60, 20));

		
		getButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				requestValue();
			}
		});
		
		setButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				requestValueChange();
			}
		});
		
		
		
		mypanel.add(label);
		mypanel.add(Box.createRigidArea(new Dimension(25,0)));
		mypanel.add(textArea);
		mypanel.add(Box.createRigidArea(new Dimension(25,0)));
		mypanel.add(setButton);
		mypanel.add(Box.createRigidArea(new Dimension(25,0)));
		mypanel.add(getButton);
		
		return mypanel;
	}
}
