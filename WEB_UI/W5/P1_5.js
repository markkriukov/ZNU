/* 
Створити функцію – калькулятор. Назва count. Функція приймає 3 параметра:

перший параметр типу Number,
другий парамерт типу Number,
третій параметр типу String,

в який передається значення операції. Значеннями операції можуть бути: «+», «-» , «*» і «/». Функція повинна повернути результат виконаної операції за допомогою return.
*/

function count(num1, num2, mark) {
    switch (mark) {
        case "+":
            return num1 + num2;
        case "-":
            return num1 - num2;
        case "*":
            return num1 * num2;
        case "/":
            return num1 / num2;
    }
}