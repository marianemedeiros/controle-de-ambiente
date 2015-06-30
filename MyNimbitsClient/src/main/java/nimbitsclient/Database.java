package nimbitsclient;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Date;

public class Database {
    private static final String DB_LOCATION = "/home/pi/pimatic-app/pimatic-database.sqlite";
    private static Connection connection = null;
    
    public static void open() throws SQLException
    {
        connection = DriverManager.getConnection("jdbc:sqlite:" + DB_LOCATION);
    }
    
    public static void close() throws SQLException
    {
        connection.close();
        connection = null;
    }
    
    //Este método retorna um ResultSet com os dados pertencentes a determinado device e sensor após certo timestamp
    public static ResultSet getData(String dataPoint, Long time) throws SQLException
    {
        String[] strings = dataPoint.split(" ");
         if(connection == null)
            open();
        String select;
        ResultSet resultSet;
        PreparedStatement stmt;
        
        select = "SELECT attributeValueNumber.value, attributeValueNumber.time FROM attributeValueNumber, deviceAttribute "
        + "WHERE deviceAttribute.deviceId = ? AND deviceAttribute.attributeName = ? "
        + "AND attributeValueNumber.time > ? " + "AND attributeValueNumber.deviceAttributeId = deviceAttribute.id";
        
        stmt = connection.prepareStatement(select);
        stmt.setString(1, strings[0]);
        stmt.setString(2, strings[1]);
        stmt.setString(3, time.toString());
        resultSet = stmt.executeQuery();
        
        return resultSet;
    }
}
