
import java.util.Properties;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author outside
 */
public class Main
{
    public static void main(String[] args)
    {
        System.loadLibrary("JOpenSurfCL");
        CSurfCL surf = new CSurfCL();
        surf.PrintCLDevices();
        surf.TestCL();
    }
}
