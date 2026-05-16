package com.example.kongzhi;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import androidx.appcompat.app.AppCompatActivity;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class MainActivity extends AppCompatActivity {

    private EditText etIp;
    private static final int PORT = 8888; // 必须和电脑端脚本的端口一致

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 绑定界面上的控件
        etIp = findViewById(R.id.et_ip);
        Button btnPrev = findViewById(R.id.btn_prev);
        Button btnPlay = findViewById(R.id.btn_play);
        Button btnNext = findViewById(R.id.btn_next);

        // 设置三个按钮的点击事件
        btnPrev.setOnClickListener(v -> sendUdpCommand("PREV"));
        btnPlay.setOnClickListener(v -> sendUdpCommand("PLAY"));
        btnNext.setOnClickListener(v -> sendUdpCommand("NEXT"));
    }

    // 发送UDP数据的核心方法
    private void sendUdpCommand(final String cmd) {
        final String ip = etIp.getText().toString().trim();

        // Android 规定网络请求必须在子线程运行，否则会报错
        new Thread(() -> {
            try {
                // 1. 创建套接字
                DatagramSocket socket = new DatagramSocket();
                // 2. 准备数据
                byte[] data = cmd.getBytes();
                InetAddress address = InetAddress.getByName(ip);
                // 3. 打包
                DatagramPacket packet = new DatagramPacket(data, data.length, address, PORT);
                // 4. 发送并关闭
                socket.send(packet);
                socket.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }
}