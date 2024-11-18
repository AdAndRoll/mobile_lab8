package com.example.myapplication

import android.os.Bundle
import android.widget.*
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
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
    private external fun removeSpecificString(ptr: Long, targetString: String)
    private external fun duplicateString(ptr: Long, targetString: String)
    private external fun getFormattedString(ptr: Long): String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Создаем экземпляры C++ классов
        counterPtr = createCounter(10)
        stringListManagerPtr = createStringListManager()

        // Корневой макет для размещения элементов
        val rootLayout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(16, 16, 16, 16)
        }
        setContentView(rootLayout)

        // --- UI для Counter ---
        val counterTextView = TextView(this).apply {
            text = "Counter: ${getCounterValue(counterPtr)}"
            textSize = 18f
        }
        rootLayout.addView(counterTextView)

        val incrementButton = Button(this).apply {
            text = "Add One"
        }
        rootLayout.addView(incrementButton)

        val resetButton = Button(this).apply {
            text = "Reset Counter"
        }
        rootLayout.addView(resetButton)

        // --- UI для StringListManager ---
        val inputText = EditText(this).apply {
            hint = "Enter a word"
        }
        rootLayout.addView(inputText)

        val buttonLayout = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
        }

        val addWordButton = Button(this).apply {
            text = "Add Word"
        }
        buttonLayout.addView(addWordButton)

        val removeWordButton = Button(this).apply {
            text = "Remove Last Word"
        }
        buttonLayout.addView(removeWordButton)

        rootLayout.addView(buttonLayout)

        val stringListLayout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
        }
        rootLayout.addView(stringListLayout)

        // --- Логика Counter ---
        resetButton.setOnClickListener {
            resetCounter(counterPtr)
            counterTextView.text = "Counter: ${getCounterValue(counterPtr)}"
        }

        incrementButton.setOnClickListener {
            incrementCounter(counterPtr)
            counterTextView.text = "Counter: ${getCounterValue(counterPtr)}"
        }

        // --- Логика StringListManager ---
        addWordButton.setOnClickListener {
            val word = inputText.text.toString().trim()
            if (word.isNotEmpty()) {
                addString(stringListManagerPtr, word)
                inputText.text.clear()
                updateStringListView(stringListLayout)
            } else {
                inputText.error = "Word cannot be empty"
            }
        }

        removeWordButton.setOnClickListener {
            removeLastString(stringListManagerPtr)
            updateStringListView(stringListLayout)
        }

        updateStringListView(stringListLayout)
    }

    /**
     * Обновляет список строк в интерфейсе.
     */
    private fun updateStringListView(layout: LinearLayout) {
        layout.removeAllViews()
        val words = getFormattedString(stringListManagerPtr)
        if (words.isNotEmpty()) {
            val wordList = words.split(", ").map { it.trim() }
            wordList.forEach { word ->
                val textView = TextView(this).apply {
                    text = word
                    textSize = 18f
                    setPadding(10, 10, 10, 10)
                    setOnClickListener { showWordOptions(word, layout) }
                }
                layout.addView(textView)
            }
        }
    }

    /**
     * Показывает диалоговое окно с выбором действий для слова.
     */
    private fun showWordOptions(word: String, layout: LinearLayout) {
        val options = arrayOf("Duplicate", "Remove")
        AlertDialog.Builder(this)
            .setTitle("Choose an action for \"$word\"")
            .setItems(options) { _, which ->
                when (which) {
                    0 -> { // Duplicate
                        duplicateString(stringListManagerPtr, word)
                        Toast.makeText(this, "Duplicated \"$word\"", Toast.LENGTH_SHORT).show()
                    }
                    1 -> { // Remove
                        removeSpecificString(stringListManagerPtr, word)
                        Toast.makeText(this, "Removed \"$word\"", Toast.LENGTH_SHORT).show()
                    }
                }
                updateStringListView(layout)
            }
            .setNegativeButton("Cancel", null)
            .show()
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
