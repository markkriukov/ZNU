/* 
Створити змінну «start» = 100 та пустий масив «result». Створити цикл, який заповнить масив «result» значеннями від 100 до 0 включно за допомогою декремента.
*/

let start = 100;
let result = [];
while(start >= 0){
    result.push(start);
    start--;
}