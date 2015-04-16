package uLinkInterface;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Scanner;

import jssc.SerialPortList;
import jssc.SerialPort;
import jssc.SerialPortException;

public class uLinkInterface {
		
	public static SerialPort serialPort;
	
	public static String portName = "COM16";
	
	public static int msgLength;


	public static void main(String[] args) throws IOException {
		    
		/*
			Scanner in = new Scanner(System.in);	
		
			System.out.println("Welcome to uLink Debugger");
	 		System.out.println("Commands:" +'\n'+ "d to confirm connection");
	 		System.out.println("t to get the network time");
	 		System.out.println("set time to set the network's time");
	 		System.out.println("m to get data from the memory");
	 		System.out.println("c to see who's connected to the network");
	 		System.out.println("s to see the current state of the grid");
	 		System.out.println("f to upload solar forecasts");
			System.out.println("r to reset the PIC");		
			System.out.println("b to get the battery energy");
			System.out.println("sb to set the battery energy");
			System.out.println("st for int data stream for debugging");			
	 		System.out.println("st fl for float data stream for debugging");
			
	 		while(true){		 	
		 		
				String input = in.nextLine();
		 		switch(input){		 		
		 		
		 			case "d":		 		
		 					System.out.println(isConnected());
		 					break;
		 			case "t":
		 					GetNodeTime();
		 				    break;
		 			case "m":
		 				    ReadMemory();					
		 				    break;
		 			case "c":
		 				    PingNetwork();
		 				    break;	 				    
		 			case "s":
		 				    GetNetworkStatus();
		 				    break;		
		 			case "f":
		 				    UploadForecasts();
    	                    break;
		 			case "set time":
		 					SetCurrentTime();
		 					break;	
		 			case "b":
		 					GetBatteryEnergy();
		 					break;
		 			case "sb":
		 					SetBatteryEnergy((float)2.3);
		 					break;		 					
		 			case "r":
		 					ResetPIC();
		 					break;
		 			case "st":
		 					StreamData();
		 					break;		 		
		 			case "st fl":
		 					StreamFloatData();
		 					break;
		 					
		 			default:
		 				    System.out.println("Not a valid input");
		 		 		
		 				break;
		 		}		 		
		 	}	 */		
	}	
	
	
	public static void GetBatteryEnergy(){		
		System.out.println("Battery Energy:");
		System.out.println(readFloat('g'));
	}	
	
	public static void SetBatteryEnergy(float value){
		Scanner in = new Scanner(System.in);			
		System.out.println("Enter the battery energy you would like: ");
		String desiredBatteryEnergy = in.nextLine();
				
		int sizeOfMessage = 6;
		byte input_buffer[] = new byte[sizeOfMessage+1];
		input_buffer[0]='b';
		input_buffer[1]=(byte) sizeOfMessage;
		
		byte byte_array[] = float2ByteArray(Float.parseFloat(desiredBatteryEnergy));
		for(int i=0;i<4;i++) input_buffer[i+2] = byte_array[i]; 	
		input_buffer[input_buffer.length - 1] = 0;			
		sendBytesToPIC(input_buffer);
		System.out.println("Battery Energy Set");
	}
	
	public static void EnableOutputStream(){		
		sendPortCommand('e');		
	}
	

	public static void UploadForecasts() throws NumberFormatException, IOException{
	    
		sendPortCommand('F',1);
			
		int i=0;
		for (String line : Files.readAllLines(Paths.get("SunnyDay.txt"), null)) {			
			SendFloatToPIC('f',Float.parseFloat(line));		
			System.out.println(i++);
			
			try{
				Thread.sleep(50);			
			}catch(InterruptedException ex){
				Thread.currentThread().interrupt();			
			} //Ends here		
		}
		
		System.out.println("Forecasts Uploaded");
	}
	
	public static void SendFloatToPIC(char command,float data){
		int sizeOfMessage=6;
		byte buffer[] = new byte[sizeOfMessage+1];
		
		byte byte_array[] = float2ByteArray(data);
				
		buffer[0]=(byte)command;
		buffer[1]=(byte)sizeOfMessage;
		for(int i=3;i>=0;i--) buffer[i+2] = byte_array[3-i];
		buffer[sizeOfMessage]=0;
		
		sendBytesToPIC(buffer);
	}
	
	
	public static void FlushFIFOBuffer(){
		try {
          serialPort.openPort();//Open serial port
          serialPort.setParams(SerialPort.BAUDRATE_57600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
        
          serialPort.writeString("f");           
          
          serialPort.closePort();//Close serial port
	    }
	 	catch (SerialPortException ex) {
	        System.out.println(ex);
	    }
		
	}
	
	public static void StreamFloatData(){
		
		serialPort = new SerialPort(portName);
		
		try {
          serialPort.openPort();//Open serial port
          serialPort.setParams(SerialPort.BAUDRATE_57600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
          
          while(true){
        	  byte byte_buffer[] = serialPort.readBytes(4);
        	  
        	 int byteToFloat[] = new int[4];
  			 for(int j=0;j<4;j++) byteToFloat[3-j]=byte_buffer[j];		
        	         	  
        	 System.out.println(BytesToFloat(byteToFloat));        	  
          }     
          
          //serialPort.closePort();//Close serial port
	    }
	 	catch (SerialPortException ex) {
	        System.out.println(ex);
	    }	 
		
	}
		
	public static void StreamData(){
		
		serialPort = new SerialPort(portName);
		
		try {
          serialPort.openPort();//Open serial port
          serialPort.setParams(SerialPort.BAUDRATE_57600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
          
          while(true){
        	  byte byte_buffer[] = serialPort.readBytes(1);
        	  System.out.println(byte_buffer[0]);        	  
          }     
          
          //serialPort.closePort();//Close serial port
	    }
	 	catch (SerialPortException ex) {
	        System.out.println(ex);
	    }
	}
	
	
	public static void SetCurrentTime(){
		Calendar cal = Calendar.getInstance();
    	
		int hours = cal.get(Calendar.HOUR_OF_DAY);
		int minutes = cal.get(Calendar.MINUTE);
		int seconds = cal.get(Calendar.SECOND);
    	
    	int SizeOfMessage = 5;		//Type, Len, Hours, Mins, Seconds
    	
    	byte msg[] = new byte[SizeOfMessage+1];
    	
    	msg[0] = 'T';
    	msg[1] = (byte) SizeOfMessage;
    	msg[2] = (byte) hours;
    	msg[3] = (byte) minutes;
    	msg[4] = (byte) seconds;
    	msg[5] = 37; //Trash Bit, EOM
    	sendBytesToPIC(msg);   	
	}
	
	
	public static void GetNodeTime(){
		char commandIn='t';
		int numBytes = 34;
		
		readCommand(commandIn, numBytes);
	}	
	
	
	
	public static void PingNetwork(){
		sendPortCommand('c');		
		byte byte_buffer[] = GetUnknownLengthMessage();
		
		System.out.println("Nodes connected:");
		for(int i=0;i<byte_buffer.length;i++){			
			System.out.println(byte_buffer[i]);			
		}
	}	
	
	public static void GetNetworkStatus(){
		sendPortCommand('s');
		
		byte byte_buffer[] = GetUnknownLengthMessage();
		
		int i=0; int 
		SizeOfNetworkStatusMessage= 7;
		while(i<=byte_buffer.length - SizeOfNetworkStatusMessage){		
			System.out.println("UID is " +byte_buffer[i]);
			
			int byteToFloat[] = new int[4];
			for(int j=0;j<4;j++) byteToFloat[3-j]=byte_buffer[j+i+1];			
			System.out.println("Current Power Consumption: " +BytesToFloat(byteToFloat));			
			System.out.println("Activities dispatched so far: "+ byte_buffer[i+5]);
			System.out.println("Has the node been shed: " +byte_buffer[i+6]);
			
			i+=SizeOfNetworkStatusMessage;
		}
	}
	
	
	public static void sendBytesToPIC(byte[] data){
		serialPort = new SerialPort(portName);
		
		try {
          serialPort.openPort();//Open serial port
          serialPort.setParams(SerialPort.BAUDRATE_57600,  
	                                 SerialPort.DATABITS_8,
	                                 SerialPort.STOPBITS_1,
	                                 SerialPort.PARITY_NONE);//Set params. Also you can set params by this string: serialPort.setParams(9600, 8, 1, 0);
          
          serialPort.writeBytes(data);
          serialPort.closePort();//Close serial port
	    }
	 	catch (SerialPortException ex) {
	        System.out.println(ex);
	    }	 
	}
	
	
	public static byte[] GetUnknownLengthMessage(){
		//System.out.println("Now trying to receive");		
		byte byte_buffer[] = null;
		
		try {
      	
          //serialPort.openPort();//Open serial port
          //serialPort.setParams(300, 8, 1, 0);//Set params.	    	  
    	  byte len[] = serialPort.readBytes(1);
    	  int msgLen = ((Byte)len[0]).intValue();    	  
    	  
    	  //System.out.println("msg len " +msgLen);
    	  
    	  byte_buffer = serialPort.readBytes(msgLen);    	 
    	  
    	  
    	  
          serialPort.closePort();//Close serial port
		}	        
		catch (SerialPortException ex) {
		  System.out.println(ex);
		}
		
		return byte_buffer;		
	}
	
	
	
	public static void ReadMemory() throws FileNotFoundException, UnsupportedEncodingException{
			
			char command = 'm';
			sendPortCommand(command);	
			
		 	
		 	//System.out.println("Now trying to receive");
			int BYTES_PER_FLOAT = 4;
			byte byte_buffer[] = null;
			
			int msg_len1 = 0;
			
	      try {
	      	
	          //serialPort.openPort();//Open serial port
	          //serialPort.setParams(300, 8, 1, 0);//Set params.	    	  
	    	  byte len[] = serialPort.readBytes(1);
	    	  msg_len1 = BYTES_PER_FLOAT*((Byte)len[0]).intValue();

	    	  System.out.println("msg leng: " +msg_len1);	    	  	  
	    	  
	    	  byte_buffer = new byte[1];	    	  
	    	  
	    	  int msg_len = BYTES_PER_FLOAT*2;	    
	    	  byte_buffer = new byte[msg_len1];	    	  
	    	  byte_buffer = serialPort.readBytes(msg_len1);//Read 10 bytes from serial port
	    	 
	    	  
	          serialPort.closePort();//Close serial port
	      }	        
	      catch (SerialPortException ex) {
	          System.out.println(ex);
	      }
		 
	      
	      if(msg_len1 > 0){
	      
		      ArrayList<Float> output = new ArrayList<Float>();
		      
		      for(int i=byte_buffer.length-1;i>=0;i-=BYTES_PER_FLOAT){
		    		    	  
		    	  int[] bytes = new int[BYTES_PER_FLOAT];
		    	  
		    	  for(int j=0;j<BYTES_PER_FLOAT;j++){
		    		  //System.out.println("Buffer val: " +byte_buffer[i-j]);
		    		  
		    		  bytes[j] = byte_buffer[i-j]; 
		    	  }    	  
		    	  output.add(BytesToFloat(bytes));
		    	  
		      }     
		      
		      PrintWriter outputFile = new PrintWriter("MicroGrid_Data.txt", "UTF-8");
		      String newLine = System.getProperty("line.separator");//This will retrieve line separator dependent on OS.		      
		      
		      for(int i=0;i<output.size();i++){
		    	  	System.out.println("One data point: " +output.get(i));
		    	  	outputFile.print(output.get(i) + newLine);
		      }
		      
		      outputFile.close();
		      
	      }
	      
		
	}
		
	
	public static boolean isConnected(){	
		String res = readCommand('d', 22);
		System.out.println("isConnected: " + res);
		return res  == "mofo connected";
	}
	
	public static void ResetPIC(){
		byte msg[] = new byte[4];
		msg[0]='r';
		msg[1]=3;
		msg[2]=0;
		msg[3]=0;
		sendBytesToPIC(msg);		
	}
	
	public static float readFloat(char command){
				
		byte[] buffer = new byte[4];
		
		try {
	          serialPort.openPort();//Open serial port
	          serialPort.setParams(SerialPort.BAUDRATE_57600, 
		                                 SerialPort.DATABITS_8,
		                                 SerialPort.STOPBITS_1,
		                                 SerialPort.PARITY_NONE);//Set params. Also you can set params by this string: serialPort.setParams(9600, 8, 1, 0);
	         int array[] = {command,3,0,0};
	         serialPort.writeIntArray(array);	          
	         buffer = serialPort.readBytes(4);	  
	         
	         serialPort.closePort();
		    }
		 	catch (SerialPortException ex) {
		        System.out.println(ex);
		    }
		
		int byteToFloat[] = new int[4];
		for(int j=0;j<4;j++) byteToFloat[3-j]=buffer[j];
		
		return BytesToFloat(byteToFloat);
	}
			
	
	public static void sendPortCommand(int command){
		serialPort = new SerialPort(portName);
		
		try {
          serialPort.openPort();//Open serial port
          serialPort.setParams(SerialPort.BAUDRATE_57600, 
	                                 SerialPort.DATABITS_8,
	                                 SerialPort.STOPBITS_1,
	                                 SerialPort.PARITY_NONE);//Set params. Also you can set params by this string: serialPort.setParams(9600, 8, 1, 0);
          
          int array[] = {command,3,0,0};
          serialPort.writeIntArray(array);       
	    }
	 	catch (SerialPortException ex) {
	        System.out.println(ex);
	    }	 
	}

	public static void sendPortCommand(int command,int closePort){
		serialPort = new SerialPort(portName);
		
		try {
          serialPort.openPort();//Open serial port
          serialPort.setParams(SerialPort.BAUDRATE_57600, 
	                                 SerialPort.DATABITS_8,
	                                 SerialPort.STOPBITS_1,
	                                 SerialPort.PARITY_NONE);//Set params. Also you can set params by this string: serialPort.setParams(9600, 8, 1, 0);
          
          int array[] = {command,3,0,0};
          serialPort.writeIntArray(array);
          if(closePort == 1) serialPort.closePort();
	    }
	 	catch (SerialPortException ex) {
	        System.out.println(ex);
	    }	 
	}
	

	public static String readCommand(char commandIn, int numBytes){	
	
		int bytesRead = numBytes;
		  
		byte[] buffer = new byte[bytesRead];
		char[] charBuffer = new char[bytesRead];
      sendPortCommand(commandIn);


      try {      	
          buffer = serialPort.readBytes(numBytes);//Read 10 bytes from serial port
          serialPort.closePort();//Close serial port
      }	        
      catch (SerialPortException ex) {
          return null;
      }	 	
      
      for(int i=0;i<buffer.length;i++){
      	charBuffer[i] = (char)buffer[i];
      }
      
      String output = new String(charBuffer);
      return output;
			
	}	
	
	
	public static float BytesToFloat(int[] bytes){		
	    int asInt = (bytes[0] & 0xFF) 
	            | ((bytes[1] & 0xFF) << 8) 
	            | ((bytes[2] & 0xFF) << 16) 
	            | ((bytes[3] & 0xFF) << 24);
	    
	    return Float.intBitsToFloat(asInt);		
	}
	
	public static byte [] float2ByteArray (float value)	{  
	     return ByteBuffer.allocate(4).putFloat(value).array();
	}
	
	
}
