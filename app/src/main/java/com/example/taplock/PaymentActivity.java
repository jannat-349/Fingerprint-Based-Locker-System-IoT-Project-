package com.example.taplock;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class PaymentActivity extends AppCompatActivity {

    private DatabaseReference fireBookingNumber, firePayAmount, fireStatus;
    private EditText editBookingNumber, editAmount;
    private Button btnPayment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.payment_page);

        // Initialize Firebase Database reference
        editBookingNumber = findViewById(R.id.edit_booking_number);
        editAmount = findViewById(R.id.edit_amount);
        btnPayment = findViewById(R.id.btn_payment);

        btnPayment.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String bookingNumber = editBookingNumber.getText().toString().trim();
                double amount = Double.parseDouble(editAmount.getText().toString().trim());

                DatabaseReference databaseReference = FirebaseDatabase.getInstance().getReference();

                // Retrieve data from Firebase
                databaseReference.addListenerForSingleValueEvent(new ValueEventListener() {
                    @Override
                    public void onDataChange(DataSnapshot dataSnapshot) {
                        Number storedBookingNumber = (Number) dataSnapshot.child("booking_number").getValue();
                        Number storedAmount = (Number) dataSnapshot.child("amount").getValue();
                        Boolean storedStatus = (Boolean) dataSnapshot.child("status").getValue();

                        // Check if the provided booking number and amount match the stored values
                        if (storedBookingNumber != null && storedAmount != null &&
                                bookingNumber.equals(String.valueOf(storedBookingNumber)) &&
                                amount == storedAmount.doubleValue()) {
                            // Update status to true
                            databaseReference.child("status").setValue(true)
                                    .addOnCompleteListener(new OnCompleteListener<Void>() {
                                        @Override
                                        public void onComplete(@NonNull Task<Void> task) {
                                            if (task.isSuccessful()) {
                                                Toast.makeText(PaymentActivity.this, "Payment Successful", Toast.LENGTH_SHORT).show();
                                                startActivity(new Intent(PaymentActivity.this, MainActivity.class));
                                                finish();
                                            } else {
                                                Toast.makeText(PaymentActivity.this, "Payment Unsuccessful", Toast.LENGTH_SHORT).show();
                                            }
                                        }
                                    });
                        } else {
                            databaseReference.child("status").setValue(false);
                            Toast.makeText(PaymentActivity.this, "Payment Unsuccessful", Toast.LENGTH_SHORT).show();
                        }
                    }

                    @Override
                    public void onCancelled(DatabaseError databaseError) {
                        // Handle error
                        Toast.makeText(PaymentActivity.this, "Error: " + databaseError.getMessage(), Toast.LENGTH_SHORT).show();
                    }
                });
            }
        });

    }
}
