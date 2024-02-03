package org.example.miniredis;

import lombok.RequiredArgsConstructor;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

@RequiredArgsConstructor
public class MiniRedisClient {
    private static final int NUMBER_OF_BYTES_IN_PROTOCOL = 4;

    private final String serverAddress;
    private final int serverPort;
    private Socket socket;
    private DataOutputStream dOut;
    private DataInputStream dIn;
    private ByteBuffer byteBuffer;

    public void connect() throws IOException {
        this.socket = new Socket(serverAddress, serverPort);
        this.dOut = new DataOutputStream(socket.getOutputStream());
        this.dIn = new DataInputStream(socket.getInputStream());
        this.byteBuffer = ByteBuffer.allocate(4096);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    }

    public String execute(String command) throws IOException, QuitException {
        if (command.trim().equals("quit")) {
            quit();
        }
        recordRequest(command);
        return readAnswer();
    }

    private void quit() throws IOException, QuitException {
        this.socket.close();
        this.dOut.close();
        this.dIn.close();
        throw new QuitException();
    }

    private void recordRequest(String command) throws IOException {
        byteBuffer.clear();
        byteBuffer.putInt(command.length());
        byteBuffer.put(NUMBER_OF_BYTES_IN_PROTOCOL, command.getBytes());
        dOut.write(byteBuffer.array(), 0, NUMBER_OF_BYTES_IN_PROTOCOL + command.length());
    }

    private String readAnswer() throws IOException {
        if (dIn.read(byteBuffer.array(), 0, 4) == -1){
            throw new IOException("Error reading the length of the message");
        }

        int length = byteBuffer.getInt(0);
        if (length > 0) {
            byte[] message = new byte[length];
            dIn.readFully(message, 0, message.length);
            return new String(message);
        }
        throw new IOException("Read length of message < 0");
    }

}
