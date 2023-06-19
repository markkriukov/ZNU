/* 
Створити глобальну змінну senseOfLife = 42. Створити функцію, яка буде приймати параметр otherSenseOfLife.
Якщо параметр передати, то функція має повернути його. А якщо не передати, то фунція має повернути глобальну змінну senseOfLife.
*/

let senseOfLife = 42;

function showVariable(otherSenseOfLife) {
    if (otherSenseOfLife) {
        return otherSenseOfLife;
    }
    return senseOfLife;
}