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
    struct StringEntry {
        std::string value;
        int id;
    };

    std::vector<StringEntry> strings;
    int idCounter = 0; // Уникальный идентификатор для каждой строки

    // Функция для переиндексации всех строк
    void reindexStrings() {
        for (int i = 0; i < strings.size(); ++i) {
            strings[i].id = i; // Обновляем ID для каждой строки
        }
    }

public:
    // Добавление строки в список
    void addString(const std::string &newString) {
        std::string lowerString = newString;
        std::transform(lowerString.begin(), lowerString.end(), lowerString.begin(), ::tolower);
        strings.push_back({lowerString, idCounter++});
    }

    // Удаление последней добавленной строки
    void removeLastString() {
        if (!strings.empty()) {
            strings.pop_back();
            reindexStrings(); // Переиндексируем после удаления
        }
    }

    // Удаление строки по ID
    void removeSpecificString(int id) {
        auto it = std::find_if(strings.begin(), strings.end(), [id](const StringEntry &entry) {
            return entry.id == id;
        });
        if (it != strings.end()) {
            strings.erase(it); // Удаляем строку по ID
            reindexStrings(); // Переиндексируем после удаления
        }
    }

    // Дублирование строки по ID
    void duplicateString(int id) {
        auto it = std::find_if(strings.begin(), strings.end(), [id](const StringEntry &entry) {
            return entry.id == id;
        });
        if (it != strings.end()) {
            strings.insert(it + 1, *it); // Вставляем копию строки после исходной
            reindexStrings(); // Переиндексируем после дублирования
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
            oss << strings[i].value;
        }

        std::string result = oss.str();
        if (!result.empty()) {
            result[0] = toupper(result[0]); // Первая буква заглавная
        }
        return result;
    }

    // Получение ID строки по индексу
    int getIdByIndex(int index) const {
        if (index >= 0 && index < strings.size()) {
            return strings[index].id;
        }
        return -1; // Если индекс выходит за границы, возвращаем -1
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

// Удаление конкретной строки по ID из списка
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_removeSpecificString(JNIEnv *env, jobject, jlong ptr, jint targetId) {
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        manager->removeSpecificString(targetId); // Удаление строки по ID
    }
}

// Дублирование строки по ID в списке
extern "C" JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_duplicateString(JNIEnv *env, jobject, jlong ptr, jint targetId) {
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        manager->duplicateString(targetId); // Дублирование строки по ID
    }
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
// Добавление функции в StringListManager для получения ID строки по индексу
extern "C" JNIEXPORT jint JNICALL
Java_com_example_myapplication_MainActivity_getIdByIndex(JNIEnv *env, jobject, jlong ptr, jint index) {
    auto *manager = reinterpret_cast<StringListManager *>(ptr);
    if (manager) {
        return manager->getIdByIndex(index); // Получаем ID строки по индексу
    }
    return -1; // Если индекс выходит за границы, возвращаем -1
}


