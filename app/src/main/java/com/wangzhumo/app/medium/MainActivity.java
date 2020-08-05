package com.wangzhumo.app.medium;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.wangzhumo.app.medium.databinding.ActivityMainBinding;
import com.wangzhumo.app.medium.widget.CustomSurfacePlayer;


public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding mainBinding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mainBinding = ActivityMainBinding.inflate(this.getLayoutInflater());
        setContentView(R.layout.activity_main);

        // create customSurfacePlayer
        CustomSurfacePlayer surfacePlayer = new CustomSurfacePlayer();
        // set current surfaceView
        surfacePlayer.setSurfaceView(mainBinding.surfaceView);
        mainBinding.btVideoStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // 开启图片选择
            }
        });
    }

}
