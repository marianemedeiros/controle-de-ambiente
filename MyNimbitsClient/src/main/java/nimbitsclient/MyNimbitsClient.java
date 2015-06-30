package nimbitsclient;

import com.nimbits.client.model.UrlContainer;
import com.nimbits.client.model.common.impl.CommonFactory;
import com.nimbits.client.model.email.EmailAddress;
import com.nimbits.client.model.point.Point;
import com.nimbits.client.model.server.Server;
import com.nimbits.client.model.server.ServerFactory;
import com.nimbits.client.model.server.apikey.AccessToken;
import com.nimbits.client.model.value.Value;
import com.nimbits.client.model.value.impl.ValueFactory;
import com.nimbits.io.NimbitsClient;
import com.nimbits.io.helper.HelperFactory;
import com.nimbits.io.helper.PointHelper;
import com.nimbits.io.helper.ValueHelper;
import com.nimbits.io.http.NimbitsClientFactory;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class MyNimbitsClient {
    private static final EmailAddress EMAIL_ADDRESS = CommonFactory.createEmailAddress("endereco@email.com"); //Endereço de email da conta do nimbits
    private static final AccessToken ACCESS_KEY = AccessToken.getInstance("chave"); //Chave criada para esta aplicação
    private static final UrlContainer INSTANCE_URL = UrlContainer.getInstance("cloud.nimbits.com"); //Instância publica do nimbits
    private static final Server SERVER = ServerFactory.getInstance(INSTANCE_URL, EMAIL_ADDRESS, ACCESS_KEY);

    private static NimbitsClient nimbitsClient;
    
    public static void main(String[] args) throws SQLException, InterruptedException
    {
        nimbitsClient = NimbitsClientFactory.getInstance(SERVER);
        while(true) //Chama método que atualiza cada sensor a cada 30s.
        {
            update("DHT11 temperature");
            update("DHT11 humidity");
            update("Light light");
            update("PIR presence");
            Thread.sleep(30000);
        }
        
        /*
        //PARA CONFERIR OS DADOS
            ValueHelper valueHelper = HelperFactory.getValueHelper(SERVER);
			System.out.println("DHT11 temperature");            
			List<Value> listValues = valueHelper.getSeries("DHT11 temperature");
            for (Value v: listValues)
                System.out.println("Valor:" + v.getDoubleValue() + " Timestamp:" + String.valueOf(v.getTimestamp()));
        
			System.out.println("DHT11 humidity");  
			listValues = valueHelper.getSeries("DHT11 humidity");
            for (Value v: listValues)
                System.out.println("Valor:" + v.getDoubleValue() + " Timestamp:" + String.valueOf(v.getTimestamp()));

			System.out.println("Light light");    
			listValues = valueHelper.getSeries("Light light");
            for (Value v: listValues)
                System.out.println("Valor:" + v.getDoubleValue() + " Timestamp:" + String.valueOf(v.getTimestamp()));

			System.out.println("PIR presence");
			listValues = valueHelper.getSeries("PIR presence");
            for (Value v: listValues)
                System.out.println("Valor:" + v.getDoubleValue() + " Timestamp:" + String.valueOf(v.getTimestamp()));
        */
    }
    
    public static void update(String dataPoint) throws SQLException
    {
       System.out.println("Update: " + dataPoint);
       List<Value> listValue = new ArrayList<Value>();
       Long time;

       ValueHelper valueHelper = HelperFactory.getValueHelper(SERVER);
       time = valueHelper.getValue(dataPoint).getTimestamp().getTime(); //Pega timestamp da última atualização do nimbits
       System.out.println("Último dataPoint: " + time.toString());
            
       System.out.println((new Date()).getTime());
       ResultSet rs = Database.getData(dataPoint, time); //Pega ResultSet com todos dados com timestamp > time
       System.out.println("Adicionando dados...");
       while(rs.next())
       {
           Long t = rs.getLong("time");
           Double d = rs.getDouble("value");
           System.out.println("Dado: " + d + " timestamp" + t);
           listValue.add(ValueFactory.createValueModel(d,new Date(t))); //Adiciona todos dados do ResultSet em uma lista de Values
       }
       Database.close();
       valueHelper.recordValues(dataPoint, listValue);//Grava todos Values no Nimbits
    }
}
