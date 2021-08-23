package com.example.tipcalculator;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity extends AppCompatActivity {


    @BindView(R.id.total_bill)
    EditText totalBillEditText;

    @BindView(R.id.tip)
    TextView tipPercentTextView;

    @BindView(R.id.minus_tip)
    Button minusTipButton;

    @BindView(R.id.add_tip)
    Button addTipButton;

    @BindView(R.id.minus_split)
    Button minusSplitButton;

    @BindView(R.id.add_split)
    Button addSplitButton;

    @BindView(R.id.split_bill)
    TextView noPersonsTextView;

    @BindView(R.id.total_pay)
    TextView totalPayTextView;

    @BindView(R.id.total_tip)
    TextView totalTipTextView;

    @BindView(R.id.total_per_person)
    TextView totalPerPersonTextView;

    //Fields
    double totalBill;
    double tipPercent;
    int noPersons;
    double totalPay;
    double totalTip;
    double totalPerPerson;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ButterKnife.bind(this);

        initFields();
        populateUI();
        setupButtons();
        setupWatchers();


    }

    /**
     * Method to initialize fields on startup
     */
    private void initFields() {

        tipPercent = 0.0;
        noPersons = 1;
        totalPay = 0.0;
        totalTip = 0.0;
        totalPerPerson = 0.0;

    }

    /**
     * Method to populate UI with correct values
     */
    private void populateUI() {

        tipPercentTextView.setText(Double.toString(tipPercent));
        noPersonsTextView.setText(Integer.toString(noPersons));
        populateResultsUI();
    }

    /**
     * Method to populate UI corresponding to results only
     */
    private void populateResultsUI(){

        totalPayTextView.setText(Double.toString(totalPay));
        totalTipTextView.setText(Double.toString(totalTip));
        totalPerPersonTextView.setText(Double.toString(totalPerPerson));
    }
    /**
     * Method to update the fields and populates the UI
     */
    private void updateFields(){

        updateTotalPay();
        updateTotalTip();
        updateTotalPayPerPerson();
    }

    private void updateTotalPay() {

        totalPay = totalBill + tipPercent / 100 * totalBill;
    }

    private void updateTotalTip() {

        totalTip = tipPercent / 100 * totalBill;
    }

    private void updateTotalPayPerPerson() {

        totalPerPerson = totalPay / noPersons;
    }

    private void setupButtons(){

        setupMinusTipButton();
        setupAddTipButton();
        setupMinusSplitButton();
        setupAddSplitButton();
    }


    private void setupMinusTipButton() {

        minusTipButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //Checking if fields are empty
                if (TextUtils.isEmpty(totalBillEditText.getText().toString().trim())) {

                    totalBillEditText.setError("Can't leave field empty.");
                    return;
                }

                if (tipPercent == 0)
                    return;

                tipPercent -= 1;
                updateFields();
                populateUI();

            }
        });
    }

    private void setupAddTipButton() {

        addTipButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //Checking if fields are empty
                if (TextUtils.isEmpty(totalBillEditText.getText().toString().trim())) {

                    totalBillEditText.setError("Can't leave field empty.");
                    return;
                }
                tipPercent += 1;
                updateFields();
                populateUI();

            }
        });
    }

    private void setupMinusSplitButton() {

        minusSplitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //Checking if fields are empty
                if (TextUtils.isEmpty(totalBillEditText.getText().toString().trim())) {

                    totalBillEditText.setError("Can't leave field empty.");
                    return;
                }
                if(noPersons == 1)
                return;

                noPersons -= 1;
                updateFields();
                populateUI();

            }
        });
    }


    private void setupAddSplitButton() {

        addSplitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                //Checking if fields are empty
                if (TextUtils.isEmpty(totalBillEditText.getText().toString().trim())) {

                    totalBillEditText.setError("Can't leave field empty.");
                    return;
                }
                noPersons += 1;
                updateFields();
                populateUI();

            }
        });
    }


    private void setupWatchers(){

        setTotalBillEditTextWatcher();

    }

    private void setTotalBillEditTextWatcher(){

        totalBillEditText.addTextChangedListener(new TextWatcher() {

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                //Do nothing
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
               //Do nothing
            }

            @Override
            public void afterTextChanged(Editable s) {

                if(s.toString().equals(""))
                    totalBill = 0;
                else
                    totalBill = Integer.parseInt(s.toString());
                updateFields();
                populateResultsUI();
            }
        });
    }


}
