HW1：就三個不同的矩陣相乘方式，gpt隨便生的


HW2：也是gpt生的，反正就偶數process先發送，奇數process先接收


HW3：有點忘了是不是gpt生的，反正就用二元樹的架構寫bcast


HW4：Allgather的話gpt就生不太出來，因為要先切分再決定哪一份要到哪個process，所以在數說誰要先發誰要先收的部分有點小麻煩，應該還有更好的寫法。然後還寫了講義上說的樹型算法，調用一次gather和bcast


HW5：gpt生的，根本沒聽過什麼Gauss-Seidal



Final Project：生個RSA金鑰出來，並使用 GMP 函式庫與 MPI 函式庫，透過並行化變數生成與大數乘法(karatsuba演算法)來提升運算效能。

update 2025/2/17
