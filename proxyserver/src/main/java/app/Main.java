package app;

import java.io.IOException;
import controllers.FirebaseController;

public class Main {
    public static void main(String[] args) throws IOException, InterruptedException {      
        System.out.println("FUKC!");  
        FirebaseController f = new FirebaseController();
        System.out.println("FUKC!");  
        f.ini();
        System.out.println("FUKC!");  
        System.out.println(f.setCurrentPos(0,139));
        System.out.println(f.getCurrentPos(0));
        System.out.println("FUKC!");  
        f.close();
    }
}
