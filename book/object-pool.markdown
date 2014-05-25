^title Пул объектов
^section Низкоуровневая оптимизация

## Общая мысль

*Улучшить быстродействие и оптимизировать работу с памятью путем переиспользования объектов из пула фиксированного размера вместо того, чтобы каждый раз создавать и уничтожать их порознь.*

## Предыстория

Мы работаем над визуальными эффектами для нашей игры. Когда герой наносит заклинание, сноп искр должен рассыпаться по экрану. Это задача для *системы частиц*: движок, который выпускает маленькие картинки искр и передвигает их, пока они не потухнут.

Поскольку одиночный взмах волшебной палочки может вызвать сотни искр, наша система должна быть способна очень быстро создавать и уничтожать эти частицы и при этом не вызывать дефрагментацию памяти.

### Фрагментация -- это плохо

Программирование для консолей, таких как XBox 360, больше похоже на программирование для встроенных устройств, чем на программирование для обычных компьютеров. Игры для консолей обязаны долгое время работать без сбоев и утечек памяти, а эффективные менеджеры памяти редко попадаются на пути. В этом случае фрагментация памяти -- смерти подобно.

Фрагментация означает, что свободное место в куче <span name="park">разбивается</span> на маленькие кусочки памяти, вместо одного большого свободного блока. *Общий*  объем свободной памяти может быть большой, но самый большой *непрерывный* регион может быть ужасно маленьким. Например, может быть 14 байтов свободно, но они состоят из двух 7-байтных кусков с большим разрывом между собой. Если мы захотим выделить 12 байт, то потерпим неудачу. И все, никаких звездочек больше на экране.

<aside name="park">

Это как парковаться на улице, где уже есть куча припаркованных автомобилей. Если бы их сдвинуть вплотную, то место быстро бы нашлось. Но машины стоят слишком широко, и втиснуться просто некуда.

</aside>

<span name="heap"></span>

<img src="images/object-pool-heap-fragment.png" />

<aside name="heap">

Тут объясняется, как память становиться фрагментированной, и как можно потерпеть неудачу, пытаясь разместить объект, для которого, вроде бы, хватает места.

</aside>

Даже если фрагментация несильная, она все равно может <span name="soak">постепенно</span> превратить память в бесполезный кусок сыра с открытыми дырками, что повредит всей игре.

<aside name="soak">

Разработчики для консолей проводят тесты, в которых они оставляют игру запущенной на несколько дней. Если игра падает, то её просто не выпускают. Хотя игра может упасть из-за редко воспроизводящегося бага, чаще всего именно чрезмерная фрагментация или утечка памяти приводят к сбою.

</aside>

### Берем лучшее от всех

Из-за фрагментации, и ещё потому что выделение памяти может быть медленной, игры заботяться о том как и когда они управляют памятью. Простое решение обычно лучше: просто выделяем себе сразу большой кусок памяти, когда игра запускается и не отпускаем его, пока она не закончится. Но это не подходит для систем, где бы должны выделять и освобождать память во время игры.

Пул объектов позволяет пользоваться плюсами обоих подходов: со стороны менеджера памяти мы выделяем один большой кусок памяти и не отпускаем, пока игра идет. А для пользователей пула объектов мы будем прозрачно создавать и уничтожать объекты.

## Принцип работы

Есть **класс пула**, который содержит коллекцию **используемых объектов**. У каждый объекта есть **статус**, который говорит нам, занят ли этот объект. Когда пул инициализируется, он создает коллекцию объектов заранее и ставит им все статус "свободен".

Когда вам нужен новый объект, вы просите пул отдать его вам. Он находит свободный объект, ставит ему статус "занят" и возвращает вам. Если объект больше не нужен, ему опять ставится статус "свободен". Таким образом, объекты могут быть заняты и освобождены без реального выделения памяти или других ресурсов.

## Когда это использовать

Этот паттерн часто используется в играх не только для визуальных эффектов и игровых объектов, но и для невидимых структур, таких как звуки. Используйте пул, если:

*   Нужно часто создавать и уничтожать объекты.

*   Все объекты одинакового размера.

*   Выделение памяти для объекта происходит медленно или может привести к фрагментации памяти.

*   Каждый объект содержит в себе доступ к ресурсу, например соединение с базой данных или сетью, который медленно открывается и может быть переиспользован.

## Что нужно помнить

Обычно можно положиться на сборщик мусора или использовать `malloc()` и `free()` для работы с памятью. Но с помощью пула объектов, вы как бы говорите, что "я знаю лучше, как эта память должна быть использована". Это значит, что вы сами будете решать все проблемы.

### Можно потерять память на объектах, которые не используются

Размер пула объектов зависит от нужд игры. При настройке, обычно очевидны ситуации, когда пул *слишком* мал (падение привлекает внимание, как ничто другое). Но лучше ещё и убедиться, что пул не *слишком большой*. Маленький пул освободит память, которую можно использовать для других прикольных штук.

### Ограничено количество объектов, которые можно одновременно активировать

По-своему, это хорошее свойство. Разбивка памяти на отдельные пулы для разных типов объектов помогает, например при массовых взрывах на экране, не съесть *всю* доступную память. Что может привести к критической проблеме, например, не получиться создать нового врага.

Тем не менее, это означает, что нужно быть готовым к ситуации, когда не удасться получить объект из пула, потому что все они заняты. Есть несколько стратегий для этой ситуации.

1.  *Заранее избежать этого.* Часто используемое "решение": настроить размер пула так, что он никогда не переполнится, неважно как пойдет игра. Для пулов с важными вещами, как враги и объекты инвентаря, это разумное решение чаще всего. Тяжело придумать, как решить проблему нехватки объекта, когда нужно создать супербосса для игрока в конце уровня. Так что лучше просто убедиться, что такого не произойдет.
    
    Минус этого решения в том, что вы, как собака на сене, будете сидеть на объектах, которые будут нужны только пару редких раз. Из-за этого пулы фиксированных размеров не всегда подходят. Например, на некоторых уровнях могут быть широко использованы визуальных эффекты, а на других -- звуковые. В каждом случае тип и размер пула придется выбирать.

2.  *Просто не создавать объект.* Звучит странно, но это имеет смысл для вещей типа системы частиц. Если все частицы используются, то экран скорее всего наполнен вспыхивающей графикой. Игрок не заметит, если следующий взрыв будет не таким мощным, как предыдущий.

3.  *Освободить занятый объект.* Допустим, есть пул звуков, и вы хотите проиграть новый звук, хотя пул заполнен. И вы *не хотите*, чтобы игрок пропустил новый звук: легко заметить, если автомат не стреляет иногда. Тогда лучшим решением будет найти самый тихий звук, который сейчас проигрывается и заменить его новым. Новый звук замаскирует пропажу старого.

    В общем, если *пропажа* существующего объекта будет менее заметной, чем *отсутствие* нового, это может оказаться верным направлением.

4.  *Увеличить размер пула.* Если ваша игра может позволить себе гибкость с памятью, то можно увеличить размер пула прямо во время работы, или создать второй, вспомогательный пул. Если при этом будет использоваться слишком много памяти, можно добавить функцию уменьшения размера пула, когда дополнительная емкость больше не нужна.

### Для каждого объекта выделяется фиксированный размер памяти

Большинство реализаций пула хранят объекты в простом массиве. Если все объекты одного типа, то это нормально. Но если типы объектов могут быть разные, или дочерний класс объекта добавляет себе поля, нужно учитывать это при выделении памяти для пула. Нужно убедиться, что пулу хватит памяти для размещения *самого большого* объекта. Иначе неожиданной большой объект затрет следующий в массиве и испортит память.

В то же время, когда размер объекта варьируется, зря используется память. Место под каждый объект увеличено, поскольку большой объект может попасть в любое место. И если больших объектов мало, то память теряется каждый раз, когда вы помещаете в пул маленький объект. Это как проходить осмотр в аэропорту и положить свои ключи в поддон для чемодана.

Если у вас такая ситуация, то можно рассмотреть возможность разделить пул на <span name="pools">отдельные</span> пулы для объектов разного размера -- большие, как для чемоданов, и маленькие, как для ключей.

<aside name="pools">

Это общий подход для реализации ориентированных на скорость менеджеров памяти. Менеджер имеет несколько пулов для блоков разного размера. Когда вы запрашиваете блок памяти, он выбирается из пула подходящего размера.

</aside>

### Объекты не очищаются автоматически

У менеджеров памяти есть специальная функция в режиме отладки, когда они заполняют только что выделенную или освобожденную память специальными значениями, типа `0xdeadbeef`. Это помогает вытащить наружу сложные шибки, которые вызваны использованием переменных, которые не инициализированы, или используют память после того, как она была освобождена.

Поскольку наш пул объектов не пользуется менеджером памяти, мы теряем эту полезную функцию. Хуже того, память под новый объект содержит данные предыдущего объекта. Из-за этого почти невозможно узнать, что вы забыли инициализировать объект: память хранит старые значения от прошлого объекта, которые выглядят *практически* корректными.

Следует внимательно следить за тем, что код, который создает объект в пуле *полностью* инициализирует объект. Возможно стоит даже потратить время на добавление специального поведения, которое <span name="clear">очищает</span> память перед тем, как запросят новый объект.

<aside name="clear">

Почту за честь, если вы будете использовать `0x1deadb0b`.

</aside>

### Объекты занимают память, даже если не используются

Пулы объектов редко встречаются в системах с автоматической сборкой мусора, потому что менеджер сам заботиться о том, чтобы не было дефрагментации памяти. Но пулы все равно пригодятся, если нужно нивелировать затраты на размещение и уничтожение ресурсов, особенно на мобильных устройствах с медленными процессорами и простыми стратегиями сборки мусора.

Если вы используете пул объектов, может возникнуть потенциальный конфликт. Поскольку пул не уничтожает объекты на самом деле, они все ещё находятся в памяти. И если они содержат ссылки на *другие* объекты, это не позволит сборщику собрать неиспользуемую память. Чтобы избежать такого поведения, очищайте ссылки в объекте, когда возвращаете его в пул.

## Пример

Системы частиц реального мира чаще всего добавляют гравитацию, ветер, трение и другие физические эффекты. Наш самый простой пример будет просто двигать частицы по прямой линии в течении пары кадров и убивать их после этого. Для блокбастера не подходит, но для демонстрации работы пула и как его использовать -- вполне.

Начнем с минимального кода. Сперва, маленький класс для частицы:

^code 1

Конструктор по умолчанию инициализирует частицу в состояние "свободна". Вызов метода `init()` позже переводит частицу в живое состояние.

Частицы анимированы с помощью метода, логично названного `animate()`, который вызывается один раз каждый кадр.

Пулу необходимо знать, какие частицы свободны для использования. Это можно узнать, вызвав функцию `inUse()` у частицы. Эта функция использует факт, что  частицы живут несколько кадров, и можно использовать переменную `_framesLeft` чтобы понять, сколько частиц свободно без добавления отдельного флага.

Класс пула также прост:

^code 2

Функция `create()`  позволяет внешнему коду создавать новые частицы. Игра вызывает <span name="update">`animate()`</span> каждый кадр. Которая, в свою очередь, вызвает метод анимации у каждой частицы в пуле.

<aside name="update">

Метод `animate()` -- это пример паттерна <a href="update-method.html" class="pattern">Update Method</a>.

</aside>

Сами частицы храняться в массиве фиксированного размера внутри класса. В этой простой реализации, размер пула известен заранее, в объявлении класса. Но можно и определить его снаружи и использовать динамические массивы, или использовать шаблонные реализации с размером.

Создание новой частицы довольно просто:

^code 3

Проходим по всему пулу и ищем первую свободную. Инициализируем её и все. В этом варианте, если свободной частицы нет, то мы просто не создадим новую.

Мы коснулись только самой простой системы частиц, не затрагивая систему отображения, конечно же. Мы можем сейчас создать пул и несколько частиц, которые автоматически деактивируются, когда их время закончится.

Этого достаточно, чтобы выпустить игру, но внимательный глаз заметит, что создание новой частицы требует <span name="create">прохода</span> по массиву, возможно полного прохода, до тех пор, пока мы не найдем пустую частицу. Если пул достаточно большой и заполнен почти полностью, то этот процесс затянется. Посмотрим, как мы можем этого избежать.

<aside name="create">

Создание частицы имеет сложность O(n). Это для тех, кто помнит, что это значит.

</aside>

### Список свободных объектов

If we don't want to waste time *finding* free particles, the obvious answer is
to not lose track of them. We could store a separate list of pointers to each
unused particle. Then, when we need to create a particle, we just remove the
first pointer from the list and reuse the particle it points to.

Unfortunately, this would require us to maintain an entire separate array with
as many pointers as there are objects in the pool. After all, when we first
create the pool, *all* particles are unused, so the list would initially have a
pointer to every object in the pool.

It would be nice to fix our performance problems *without* sacrificing any
memory. Conveniently, there is some memory already lying around we can borrow:
the data for the unused particles themselves.

When a particle isn't in use, most of its state is irrelevant. Its position and
velocity aren't being used. The only state it needs is the stuff required to
tell if it's dead. In our example, that's the `_framesLeft` member. All those
other bits can be reused. Here's a revised particle:

^code 4

We've gotten all of the member variables except for `framesLeft_` and moved them
into a `live` struct inside a `state_` <span name="union">union</span>. This
struct holds the particle's state when it's being animated. When the particle is
unused, the other case of the union, the `next` member, is used. It holds a
pointer to the next available particle after this one.

<aside name="union">

Unions don't seem to be used that often these days, so the syntax may be
unfamiliar to you. If you're on a game team, you've probably got a "memory
guru", that beleaguered compatriot whose job it is to come up with a solution
when the game has inevitably blown its memory budget. Ask them about unions.
They'll know all about them and other fun bit-packing tricks.

</aside>

We can use these pointers to build a linked list that chains together every
unused particle in the pool. We have the list of available particles we need,
but did't need to use any additional memory. Instead, we cannibalize the memory
of the dead particles themselves to store the list.

This clever technique is called a [*free
list*](http://en.wikipedia.org/wiki/Free_list). For it to work, we need to make
sure the pointers are initialized correctly and are maintained when particles
are created and destroyed. And, of course, we need to keep track of the list's
head:

^code 5

When a pool is first created, *all* of the particles are available, so
our free list should thread through the entire pool. The pool
constructor sets that up:

^code 6

Now to create a new particle we just jump directly to the <span name="first">first</span>
available one:

<aside name="first">

O(1) complexity, baby! Now we're cooking!

</aside>

^code 7

When a particle gives up the ghost we just thread it back onto the
list:

^code 8

There you go, a nice little object pool with constant-time creation
and deletion.

## Design Decisions

As you've seen, the simplest object pool implementation is almost
trivial: create an array of objects and reinitialize them as needed.
Production code is rarely that minimal. There are several ways to
expand on that to make the pool more generic, safer to use, or easier
to maintain. As you implement pools in your games, you'll need to
answer these questions:

### Are objects coupled to the pool?

The first question you'll run into when writing an object pool is
whether the objects themselves know they are in a pool. Most of
the time they will, but you won't have that luxury when writing a
generic pool class that can hold arbitrary objects.

* **If objects are coupled to the pool:**

     *  *The implementation is simpler.* You can simply put an "in
        use" flag or function in your pooled object and be done with
        it.

     *  *You can ensure that the objects can only be created by the pool.*
        In C++, a simple way to do this is to make the pool class a friend
        of the object class, and then make the object's constructor
        private.

        ^code 10

        This relationship documents the intended way to use the class and
        ensures your users don't create objects that aren't tracked by the
        pool.

     *  *You may be able to avoid storing an explicit "in use"
        flag.* Many objects already retain some state that could be used
        to tell whether it is alive or not. For example, a particle may be
        available for reuse if its current position is offscreen. If the
        object class knows it may be used in a pool, it can provide an
        `inUse()` method to query that state. This saves the pool from
        having to burn some extra memory storing a bunch of "in
        use" flags.

* **If objects are not coupled to the pool:**

     *  *Objects of any type can be pooled.* This is the big advantage. By
        decoupling objects from the pool, you may be able to implement a
        generic reusable pool class.

     *  *The "in use" state must be tracked outside the
        objects.* The simplest way to do this is by creating a separate
        bit field.

        ^code 11

### What is responsible for initializing the reused objects?

In order to reuse an existing object, it must be reinitialized with
new state. A key question here is whether to reinitialize the object
inside the pool class or outside.

* **If the pool reinitializes internally:**

     *  *The pool can completely encapsulate its objects*. Depending on
        the other capabilities your objects need, you may be able to keep
        them completely internal to the pool. This makes sure that other
        code doesn't maintain references to objects that could be
        unexpectedly reused.

     *  *The pool is tied to how objects are initialized*. A pooled object
        may offer multiple functions that initialize it. If the pool
        manages initialization, its interface needs to support all of
        those and forward them to the object.

        ^code 12

* **If outside code initializes the object:**

     *  *The pool's interface can be simpler.* Instead of offering
        multiple functions to cover each way an object can be initialized,
        the pool can simply return a reference to the new object.

        ^code 13

        The caller can then initialize the object by calling any method
        the object exposes.

        ^code 14

     *  *Outside code may need to handle the failure to create a new
        object.* The previous example assumes that `create()` will always
        successfully return a pointer to an object. If the pool is full,
        though, it may return `NULL` instead. To be safe, you'll need to
        check for that before you try to initialize the object.

        ^code 15

## See Also

*   This looks a lot like the <a class="gof-pattern" href="flyweight.html">
    Flyweight pattern</a>. Both maintain a collection of reusable objects. The
    difference is what "reuse" means. Flyweight objects are reused by sharing
    the same instance between multiple owners *simultaneously*. It avoids
    *duplicate* memory usage by using the same object in multiple contexts.

    The objects in a pool get reused too, but only over time. "Reuse" in the
    context of an object pool means reclaiming the memory for an object
    *after* the original owner is done with it. With an object pool, there
    isn't any expectation that an object will be shared within its
    lifetime.

*   Packing a bunch of objects of the same type together in memory helps keep
    your CPU cache full as you iterate over those objects. The
    <a class="pattern" href="data-locality.html">Data Locality pattern</a>
    is all about that.
