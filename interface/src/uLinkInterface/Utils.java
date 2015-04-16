package uLinkInterface;

import java.nio.ByteBuffer;

public class Utils {
	public static Float BytesToFloat(byte[] bytes){		
	    int asInt = (bytes[0] & 0xFF) 
	            | ((bytes[1] & 0xFF) << 8) 
	            | ((bytes[2] & 0xFF) << 16) 
	            | ((bytes[3] & 0xFF) << 24);
	    
	    return Float.intBitsToFloat(asInt);		
	}
	
	public static Short BytesToShort(byte[] bytes) {
	    int asInt = ((int)bytes[0] & 0xFF) 
	                  | (((int)bytes[1] & 0xFF) << 8);
	    return new Short((short)asInt);
	}
	
	public static void stopAndEducate(String message) {
		System.out.println(message);
		System.exit(1);
	}
	
	public static byte [] FloatToBytes (float value)	{  
	     return ByteBuffer.allocate(4).putFloat(value).array();
	}
	
	public static byte [] ShortToBytes (short value)	{  
	     return ByteBuffer.allocate(4).putShort(value).array();
	}
	
	
}
