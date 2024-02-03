package org.example;

import org.example.miniredis.MiniRedisClient;
import org.example.miniredis.QuitException;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        final String serverAddress = "localhost";
        final int serverPort = 2343;
        Scanner scanner = new Scanner(System.in);

        MiniRedisClient miniRedisClient = new MiniRedisClient(serverAddress, serverPort);
        try {
            miniRedisClient.connect();
            while (true){
                String userInput = scanner.nextLine();
                String res = miniRedisClient.execute(userInput);
                System.out.println(res);
            }
        } catch (IOException e){
            e.printStackTrace();
        } catch (QuitException e){
            System.out.println("Disconnect...");
        }

    }
}