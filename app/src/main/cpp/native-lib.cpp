#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

// Определяем класс Counter
class Counter {
private:
    int x;

public:
    explicit Counter(int initialValue = 0) : x(initialValue) {}

    void PlusOne() {
        x++;
    }

    int GetValue() const {
        return x;
    }

    void Reset() {
        x = 0;
    }
};

// Определяем класс StringListManager
class StringListManager {
private:
    std::vector<std::string> strings;
    int id =0;
public:
    // Добавление строки в список
    void addString(const std::string &newString) {
        std::string lowerString = newString;
        std::transform(lowerString.begin(), lowerString.end(), lowerString.begin(), ::tolower);
        strings.push_back(lowerString);
        this->id= this->id+1;
    }

    // Удаление последней добавленной строки
    void removeLastString() {
        if (!strings.empty()) {
            strings.pop_back();
        }
    }

    // Удаление конкретной строки
    void removeSpecificString(const std::string &targetString) {
        auto it = std::find(strings.begin(), strings.end(), targetString);
        if (it != strings.end()) {
            strings.erase(it); // Удаляем только то слово, на которое было нажато
        }
    }

    // Дублирование конкретной строки
    void duplicateString(const std::string &targetString) {
        auto it = std::find(strings.begin(), strings.end(), targetString);
        if (it != strings.end()) {
            strings.insert(it + 1, targetString); // Вставляем копию строки после исходной
        }
    }

    // Получение всех строк в формате одной строки, через запятую
    std::string getFormattedString() const {
        if (strings.empty()) {
            return "";
        }

        std::ostringstream oss;
        for (size_t i = 0; i < strings.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << strings[i];
        }

        std::string result = oss.str();
        if (!result.empty()) {
            result[0] = toupper(result[0]); // Первая буква заглавная
        }
        return result;
    }
};

// JNI функции

// Генерация строки "Hello from C++"
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

// --- Counter методы ---

// Создание экземпляра Counter
extern "C" JNIEXPORT jlong JNICALL
Java_com_example_myapplication_MainActivity_createCounter(JNIEnv *env, jobject, jint initialValue) {
    auto *counter = new Counter(initialValue);
    return reinterpret_cast<jlong>(counter);
}

// Увеличение значения счётчика
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_incrementCounter(JNIEnv *env, jobject, jlong instancePtr) {
    auto *counter = reinterpret_cast<Counter *>(instancePtr);
    counter->PlusOne();
}

// Получение значения счётчика
extern "C" JNIEXPORT jint JNICALL
Java_com_example_myapplication_MainActivity_getCounterValue(JNIEnv *env, jobject, jlong instancePtr) {
    auto *counter = reinterpret_cast<Counter *>(instancePtr);
    return counter->GetValue();
}

// Сброс значения счётчика
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_resetCounter(JNIEnv *env, jobject, jlong instancePtr) {
    auto *counter = reinterpret_cast<Counter *>(instancePtr);
    if (counter) {
        counter->Reset();
    }
}

// Удаление экземпляра Counter
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_deleteCounter(JNIEnv *env, jobject, jlong instancePtr) {
    auto *counter = reinterpret_cast<Counter *>(instancePtr);
    delete counter;
}

// --- StringListManager методы ---

// Создание экземпляра StringListManager
extern "C" JNIEXPORT jlong JNICALL
Java_com_example_myapplication_MainActivity_createStringListManager(JNIEnv *env, jobject) {
    return reinterpret_cast<jlong>(new StringListManager());
}

// Удаление экземпляра StringListManager
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_deleteStringListManager(JNIEnv *env, jobject, jlong ptr) {
    delete reinterpret_cast<StringListManager *>(ptr);
}

// Добавление строки в список
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_addString(JNIEnv *env, jobject, jlong ptr, jstring newString) {
    const char *nativeString = env->GetStringUTFChars(newString, nullptr);
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        manager->addString(nativeString);
    }
    env->ReleaseStringUTFChars(newString, nativeString);
}

// Удаление последней строки из списка
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_removeLastString(JNIEnv *env, jobject, jlong ptr) {
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        manager->removeLastString();
    }
}

// Удаление конкретной строки из списка
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_removeSpecificString(JNIEnv *env, jobject, jlong ptr, jstring targetString) {
    const char *nativeString = env->GetStringUTFChars(targetString, nullptr);
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        manager->removeSpecificString(nativeString); // Удаление только того слова, которое выбрал пользователь
    }
    env->ReleaseStringUTFChars(targetString, nativeString);
}

// Дублирование строки в списке
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_duplicateString(JNIEnv *env, jobject, jlong ptr, jstring targetString) {
    const char *nativeString = env->GetStringUTFChars(targetString, nullptr);
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        manager->duplicateString(nativeString);
    }
    env->ReleaseStringUTFChars(targetString, nativeString);
}

// Получение отформатированной строки со всеми элементами списка
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_getFormattedString(JNIEnv *env, jobject, jlong ptr) {
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        std::string formattedString = manager->getFormattedString();
        return env->NewStringUTF(formattedString.c_str());
    }
    return env->NewStringUTF("");
}

