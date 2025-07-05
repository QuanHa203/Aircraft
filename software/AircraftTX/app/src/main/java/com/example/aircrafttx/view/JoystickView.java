package com.example.aircrafttx.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class JoystickView extends View {

    private float centerX, centerY, baseRadius, hatRadius;
    private float touchX, touchY;
    float xPercent, yPercent;

    private boolean enableX = true;
    private boolean enableY = true;
    private boolean autoReturnXToCenter = true;
    private boolean autoReturnYToCenter = true;

    private final Paint basePaint = new Paint();
    private final Paint hatPaint = new Paint();

    public interface JoystickListener {
        void onJoystickMoved(float xPercent, float yPercent);
    }

    private JoystickListener joystickListener;

    public void setEnableXAxis(boolean enable) {
        this.enableX = enable;
        invalidate();
    }

    public void setEnableYAxis(boolean enable) {
        this.enableY = enable;
        invalidate();
    }

    public void setAutoReturnXToCenter(boolean enable) {
        this.autoReturnXToCenter = enable;
    }

    public void setAutoReturnYToCenter(boolean enable) {
        this.autoReturnYToCenter = enable;
    }

    public void setJoystickListener(JoystickListener listener) {
        this.joystickListener = listener;
    }


    public JoystickView(Context context, AttributeSet attrs) {
        super(context, attrs);
        basePaint.setColor(Color.GRAY);
        hatPaint.setColor(Color.BLUE);
        basePaint.setAntiAlias(true);
        hatPaint.setAntiAlias(true);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        centerX = getWidth() / 2f;
        centerY = getHeight() / 2f;
        baseRadius = Math.min(getWidth(), getHeight()) / 3f;
        hatRadius = baseRadius / 2f;

        // Giá trị mặc định nếu chưa chạm
        if (touchX == 0 && touchY == 0) {
            touchX = centerX;
            touchY = centerY;
        }

        canvas.drawCircle(centerX, centerY, baseRadius, basePaint);
        canvas.drawCircle(touchX, touchY, hatRadius, hatPaint);
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float dx = event.getX() - centerX;
        float dy = event.getY() - centerY;
        float distance = (float) Math.sqrt(dx * dx + dy * dy);

        float newX = enableX ? event.getX() : centerX;
        float newY = enableY ? event.getY() : centerY;

        if (event.getAction() != MotionEvent.ACTION_UP) {
            if (distance < baseRadius) {
                touchX = newX;
                touchY = newY;
            }
            else {
                float ratio = baseRadius / distance;
                touchX = centerX + (enableX ? dx * ratio : 0);
                touchY = centerY + (enableY ? dy * ratio : 0);
            }

            xPercent = enableX ? (touchX - centerX) / baseRadius : 0;
            yPercent = enableY ? (touchY - centerY) / baseRadius : 0;

            if (joystickListener != null)
                joystickListener.onJoystickMoved(xPercent, yPercent);

        }
        else {
            if (autoReturnXToCenter && autoReturnYToCenter) {
                touchX = centerX;
                touchY = centerY;

                if (joystickListener != null)
                    joystickListener.onJoystickMoved(0, 0);
            }
            else if (autoReturnXToCenter) {
                touchX = centerX;

                if (joystickListener != null)
                    joystickListener.onJoystickMoved(0, yPercent);
            }
            else if (autoReturnYToCenter) {
                touchY = centerY;

                if (joystickListener != null)
                    joystickListener.onJoystickMoved(xPercent, 0);
            }
        }

        invalidate();
        return true;
    }
}