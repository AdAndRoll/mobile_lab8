package com.example.myapplication

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import com.example.myapplication.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    // Указатель на экземпляр C++ класса Counter
    private var counterPtr: Long = 0

    // Указатель на экземпляр C++ класса StringListManager
    private var stringListManagerPtr: Long = 0

    // Native методы для Counter
    private external fun createCounter(initialValue: Int): Long
    private external fun incrementCounter(instancePtr: Long)
    private external fun getCounterValue(instancePtr: Long): Int
    private external fun deleteCounter(instancePtr: Long)
    private external fun resetCounter(instancePtr: Long)

    // Native методы для StringListManager
    private external fun createStringListManager(): Long
    private external fun deleteStringListManager(ptr: Long)
    private external fun addString(ptr: Long, newString: String)
    private external fun removeLastString(ptr: Long)
    private external fun getFormattedString(ptr: Long): String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Создаем экземпляры C++ классов
        counterPtr = createCounter(10)
        stringListManagerPtr = createStringListManager()

        // Инициализируем UI элементы для Counter
        val counterTextView: TextView = binding.sampleText
        val incrementButton: Button = Button(this).apply {
            text = "Add One"
        }
        binding.root.addView(incrementButton)

        val resetButton: Button = Button(this).apply {
            text = "Reset Counter"
        }
        binding.root.addView(resetButton)

        // Инициализируем UI элементы для StringListManager
        val inputText: EditText = binding.inputText
        val addWordButton: Button = Button(this).apply {
            text = "Add Word"
        }
        binding.root.addView(addWordButton)

        val removeWordButton: Button = Button(this).apply {
            text = "Remove Last Word"
        }
        binding.root.addView(removeWordButton)

        val stringListTextView: TextView = TextView(this)
        binding.root.addView(stringListTextView)

        // Обработчик для кнопки сброса счётчика
        resetButton.setOnClickListener {
            resetCounter(counterPtr)
            counterTextView.text = "Counter: ${getCounterValue(counterPtr)}"
        }

        // Отображаем начальное значение счётчика
        counterTextView.text = "Counter: ${getCounterValue(counterPtr)}"

        // Обновляем значение счётчика при нажатии на кнопку
        incrementButton.setOnClickListener {
            incrementCounter(counterPtr)
            counterTextView.text = "Counter: ${getCounterValue(counterPtr)}"
        }

        // Обработчик для кнопки добавления строки в StringListManager
        addWordButton.setOnClickListener {
            val word = inputText.text.toString().trim()
            if (word.isNotEmpty()) {
                addString(stringListManagerPtr, word)
                inputText.text.clear()
                stringListTextView.text = getFormattedString(stringListManagerPtr)
            } else {
                inputText.error = "Word cannot be empty"
            }
        }

        // Обработчик для кнопки удаления последнего слова из StringListManager
        removeWordButton.setOnClickListener {
            removeLastString(stringListManagerPtr)
            stringListTextView.text = getFormattedString(stringListManagerPtr)
        }

        // Изначально выводим список строк
        stringListTextView.text = getFormattedString(stringListManagerPtr)
    }

    override fun onDestroy() {
        super.onDestroy()
        // Удаляем экземпляры C++ классов
        if (counterPtr != 0L) {
            deleteCounter(counterPtr)
            counterPtr = 0
        }
        if (stringListManagerPtr != 0L) {
            deleteStringListManager(stringListManagerPtr)
            stringListManagerPtr = 0
        }
    }

    companion object {
        init {
            System.loadLibrary("myapplication")
        }
    }
}
