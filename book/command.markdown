^title Команда
^section Повторяем паттерны проектирования

Команда - один из моих любимых паттернов. В большинстве больших программ, которые я пишу, будь то игры или что-то другое, употребляется этот паттерн. При использовании в нужном месте он аккуратно распутывает действительно кривой код. Для такого раздутого паттерна у "Банды четырех" есть предсказуемо малопонятное определение:

> Инкапсулируйте запрос в качестве объекта, тем самым позволяя пользователям 
параметризовать клиентов с различными запросами, поставьте в очередь или 
запишите запросы в журнал и организуйте поддержку отменяемых операций.

Согласитесь, страшное предложение? Прежде всего, оно искажает все, что бы эта метафора ни пыталась заключить. Вне странного мира программного обеспечения, где слова могут означать всё, что угодно, "клиент" -- это человек, с которым ведут бизнес. Вплоть до настоящего времени людей нельзя было "параметризовать".

Далее, оставшаяся часть предложения - это просто список того, к чему, вероятно, можно применить данный паттерн. Не очень понятно, если вашего сценария использования нет в этом списке. *Мой* сжатый слоган для паттерна "Команда" будет звучать так:

**Команда - это *<span name="latin">материализованный</span> вызов метода*.**

<aside name="latin">

"Материализовать" ("reify") происходит от латинского "res" — "вещь", с добавлением английского суффикса
"&ndash;fy". Поэтому это слово буквально значит "овеществлять" ("thingify"), что, честно говоря, гораздо больше подошло бы в качестве определения.

</aside>

Конечно, под словом "сжатый" часто подразумевается "предельно краткий", поэтому определение паттерна, вероятно, не очень улучшилось. Позвольте мне предложить немного более развернутое описание. "Материализовать" (если вы никогда не слышали этого слова) означает "сделать реальным". Другим термином для определения слова "материализовать" является сделать что-то объектом "первого класса".

<aside name="reflection">

*Системы рефлексии* в некоторых языках программирования позволяют работать с типами в программе императивно во время выполнения. Можно получить объект, представляющий класс другого объекта, и поиграть с ним, чтобы увидеть, что умеет делать тип. Другими словами, рефлексия - это *материализованная система типов*.

</aside>

Оба выражения подразумевают взятие некоего <span name="reflection">*концепта*</span> и превращение его в кусок *данных* -- объект -- который можно поместить в переменную, передать на вход функции и т.д. Таким образом, называя паттерн "Команда" "вызовом материализованного метода", я имею в виду, что это вызов метода, обернутый в объект.

Это во многом похоже на "функцию обратного вызова", "функцию первого класса", "указатель на функцию", "замыкание" или "частично применяемую функцию", в зависимости от того, на каком языке программирования вы пишете, и, на самом деле, все это растения из одного сада. Далее "Банда четырёх" пишет:

> Команды - это объектно-ориентированная замена функций обратного вызова.

Это определение стало бы намного более удачным для паттерна, нежели то, которое они выбрали.

Но все это абстрактно и туманно. Я люблю начинать главы с конкретики, а в этот раз у меня это не получилось. Чтобы восполнить эту досадную оплошность, с текущего момента и дальше будут только сухие примеры, в которых команды найдут блестящее применение.

## Настройка ввода

В любой игре где-то есть кусок кода, который считывает необработанные данные пользователя с устройств ввода -- нажатия кнопок, клавиатурные события, щелчки мышью - что угодно. Он каждый раз берет входные данные и преобразует их в имеющее смысл действие в игре:

<img src="images/command-buttons-one.png" alt="Контроллер с кнопкой A, привязанной к swapWeapon(), B, привязанной к lurch(), X, привязанной к jump(), и Y, привязанной к fireGun()." />

Самая простая реализация выглядит примерно так:

<span name="lurch"></span>

^code handle-input

<aside name="lurch">

Профессиональный совет: не нажимайте "B" слишком часто.

</aside>

Эта функция обычно вызывается раз в кадр паттерном <a class="pattern"
href="game-loop.html">"Игровой цикл"</a>, и я уверен, вы сможете понять, что она делает. Она работает, если мы хотим жестко привязать кнопки ввода к игровым действиям, но многие игры разрешают пользователям *настроить* привязку кнопок.

Чтобы поддерживать это, нужно поменять прямые вызовы `jump()` и `fireGun()` на что-то, что можно изменить. "Изменить" звучит во многом, как определить переменную, поэтому нужен *объект*, который можно использовать для отображения игрового действия.
Встречайте: паттерн "Команда".

Определим базовый класс, представляющий срабатываемую игровую команду:

<span name="one-method"></span>

^code command

<aside name="one-method">

Если есть интерфейс с единственным методом, который ничего не возвращает, очень вероятно, что это паттерн "Команда".

</aside>

Затем создаем подклассы для каждого из различных игровых действий:

^code command-classes

В обработчике входных данных сохраним указатель на команду для каждой кнопки:

^code input-handler-class

Теперь обработка входных данных просто делегирует полномочия:

<span name="null"></span>

^code handle-input-commands

<aside name="null">

Обратили внимание, что мы не проверяем указатель на равенство `null`? Здесь предполагается, что каждая кнопка будет иметь *некоторую* привязанную к ней команду.

Если мы хотим поддерживать бездействующие кнопки без необходимости явно проверять на равенство `null`, можно определить класс команды, чей метод `execute()` ничего не делает.
Затем вместо того, чтобы установить обработчик кнопки в `null`, делаем его указателем на тот объект.
Это паттерн, называемый [Нулевой объект](http://ru.wikipedia.org/wiki/Null_object_%28%D0%A8%D0%B0%D0%B1%D0%BB%D0%BE%D0%BD_%D0%BF%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F%29).

</aside>

Там, где при вводе использовался прямой вызов функции, теперь появился слой абстракции:

<img src="images/command-buttons-two.png" alt="Контроллер, где каждая кнопка привязана к соответствующей переменной 'button_', которая в свою очередь привязана к функции." />

В этом суть паттерна "Команда". Если вы уже видите его преимущество, рассматривайте оставшуюся часть главы, как бонус.

## Директивы для акторов

Только что определенные классы команд работают в предыдущем примере, но весьма ограничены. Проблема в том, что они предполагают существование высокоуровневых функций 
`jump()`, `fireGun()` и прочих, неявно знающих, как найти аватар игрока и заставить его танцевать, как марионетку, которой он и является.

Эта предполагаемая связь ограничивает пользу данных команд. *Единственный*, кто может подпрыгнуть по команде `JumpCommand` - это игрок. Давайте ослабим это ограничение.
Вместо вызова функций, самостоятельно находящих объект команды, *передадим на вход* объект, которым хотим управлять:

^code actor-command

Здесь `GameActor` - класс "игрового объекта", который представляет персонажа в мире игры.  Передаем его в `execute()`, чтобы унаследованная команда могла вызвать методы, применяя их к выбранному актору, например:

^code jump-actor

Теперь можно использовать этот единственный класс, чтобы заставить прыгать любого персонажа в игре. Нам просто не хватает кусочка между обработчиком пользовательского ввода и командой, который бы принимал команду и вызывал её на правильном объекте. Сначала изменим `handleInput()` так, чтобы он *возвращал* команды:

^code handle-input-return

Он не может выполнить команду немедленно, потому что не знает, какого актора передать на вход. Вот где мы воспользуемся тем фактом, что команда является материализованным вызовом -- можно *отложить* выполнение вызова.

Далее нужен какой-то код, который берет команду и запускает на акторе, представляющем игрока. Что-то вроде этого:

^code call-actor-command

Предположим, что `actor` - это ссылка на персонаж игрока, тогда этот код правильно передвигает его согласно управляющим клавишам пользователя, поэтому мы возвращаемся к тому же поведению из первого примера. Но добавление уровня абстракции между командой и выполняющим ее актором дает небольшое изящное преимущество: *теперь можно позволить игроку управлять любым актором в игре, просто сменив актора, на котором выполняются команды.*

На практике это не распространенная возможность, но есть аналогичный сценарий использования, *действительно* часто всплывающий. До сих пор мы рассматривали только управляемого игроком персонажа, но что насчет других акторов в мире? Они управляются искусственным интеллектом игры. Можно использовать тот же самый паттерн "Команда" в качестве интерфейса между движком искусственного интеллекта и акторами: код искусственного интеллекта просто генерирует  объекты `Command`.

Здесь декомпозиция искусственного интеллекта, выбирающего команды, и кода актора, выполняющего их, предоставляет хороший уровень гибкости. Можно использовать различные модули искусственного интеллекта для различных акторов. Или можно смешать и сопоставить куски искусственного интеллекта для различных видов поведения. Хотите получить более агрессивного врага? Просто подключите более агрессивный искусственный интеллект, чтобы генерировать команды для него. Фактически можно даже прицепить искусственный интеллект к персонажу *игрока*, что может быть полезно для таких вещей, как демо-режим, когда игра должна работать на автопилоте.

<span name="queue">Путем</span> превращения команд, управляющих актором, в объекты первого класса, мы убрали жесткую зависимость от прямого вызова метода.
Вместо этого получилось что-то вроде очереди или потока команд:

<aside name="queue">

Намного больше о том, что может сделать постановка в очередь, вы найдете в главе <a href="event-queue.html"
class="pattern">"Очередь событий"</a>.

</aside>

<span name="stream"></span>

<img src="images/command-stream.png" alt="Поток, соединяющий искусственный интеллект с актором." />

<aside name="stream">

Почему я чувствую потребность нарисовать картинку "потока" для вас? И почему он выглядит, как труба?

</aside>

Некоторый код (обработчик пользовательского ввода или искусственный интеллект) <span name="network">генерирует</span> команды и помещает их в поток. Другой код (диспетчер или сам актор) получает команды и вызывает их. Поместив очередь в центр, мы сделали декомпозицию генератора на одном конце от потребителя на другом.

<aside name="network">

Если взять эти команды и сделать их *сериализуемыми*, то можно послать поток из них по сети. Мы можем взять данные, введенные игроком, послать их по сети на другую машину и затем повторно воспроизвести их. Это важная часть создания сетевой многопользовательской игры.

</aside>

## Undo and Redo

The last example is the most well-known use of this pattern. If a command object
can *do* things, it's a small step for it to be able to *undo* them. Undo is
used in some strategy games where you can roll back moves that you didn't like.
It's *de rigueur* in tools that people use to *create* games. The <span
name="hate">surest way</span> to make your game designers hate you is giving
them a level editor that can't undo their fat-fingered mistakes.

<aside name="hate">

I may be speaking from experience here.

</aside>

Without the Command pattern, implementing undo is surprisingly hard. With it,
it's a piece of cake. Let's say we're making a single-player, turn-based game and
we want to let users undo moves so they can focus more on strategy and less on
guesswork.

We're conveniently already using commands to abstract input handling, so every
move the player makes is already encapsulated in them. For example, moving a
unit may look like:

^code move-unit

Note this is a little different from our previous commands. In the last example,
we wanted to *abstract* the command from the actor that it modified. In this
case, we specifically want to *bind* it to the unit being moved. An instance of
this command isn't a general "move something" operation that you could use in a
bunch of contexts; it's a specific concrete move in the game's sequence of
turns.

This highlights a variation in how the Command pattern gets implemented. In some
cases, like our first couple of examples, a command is a reusable object that
represents a *thing that can be done*. Our earlier input handler held on to a
single command object and called its `execute()` method anytime the right button
was pressed.

Here, the commands are more specific. They represent a thing that can be done at
a specific point in time. This means that the input handling code will be <span
name="free">*creating*</span> an instance of this every time the player chooses
a move. Something like:

^code get-move

<aside name="free">

Of course, in a non-garbage-collected language like C++, this means the code
executing commands will also be responsible for freeing their memory.

</aside>

The fact that commands are one-use-only will come to our advantage in a second.
To make commands undoable, we define another operation each command class needs
to implement:

^code undo-command

An `undo()` method reverses the game state changed by the corresponding
`execute()` method. Here's our previous move command with undo support:

^code undo-move-unit

Note that we added some <span name="memento">more state</span> to the class.
When a unit moves, it forgets where it used to be. If we want to be able to undo
that move, we have to remember the unit's previous position ourselves, which is
what `xBefore_` and `yBefore_` do.

<aside name="memento">

This seems like a place for the <a
href="http://en.wikipedia.org/wiki/Memento_pattern"
class="gof-pattern">Memento</a> pattern, but I haven't found it to work well.
Since commands tend to modify only a small part of an object's state,
snapshotting the rest of its data is a waste of memory. It's cheaper to
manually store only the bits you change.

<a href="http://en.wikipedia.org/wiki/Persistent_data_structure">*Persistent
data structures*</a> are another option. With these, every modification to an
object returns a new one, leaving the original unchanged. Through clever
implementation, these new objects share data with the previous ones, so it's
much cheaper than cloning the entire object.

Using a persistent data structure, each command stores a reference to the
object before the command was performed, and undo just means switching back to
the old object.

</aside>

To let the player undo a move, we keep around the last command they executed.
When they bang on Control-Z, we call that command's `undo()` method. (If they've
already undone, then it becomes "redo" and we execute the command again.)

Supporting multiple levels of undo isn't much harder. Instead of remembering the
last command, we keep a list of commands and a reference to the "current" one.
When the player executes a command, we append it to the list and point "current"
at it.

<img src="images/command-undo.png" alt="A stack of commands from older to newer. A 'current' arrow points to one command, an 'undo' arrow points to the previous one, and 'redo' points to the next." />

When the player chooses "Undo", we undo the current command and move the current
pointer back. When they choose <span name="replay">"Redo"</span>, we advance the
pointer
and then execute that command. If they choose a new command after undoing some,
everything in the list after the current command is discarded.

The first time I implemented this in a level editor, I felt like a wizard. I was
astonished at how straightforward it was and how well it worked. It takes
discipline to make sure every data modification goes through a command, but once
you do that, the rest is easy.

<aside name="replay">

Redo may not be common in games, but re-*play* is. A naïve implementation would
record the entire game state at each frame so it can be replayed, but that would
use too much memory.

Instead, many games record the set of commands every entity performed each
frame. To replay the game, the engine just runs the normal game simulation,
executing the pre-recorded commands.

</aside>

## Classy and Dysfunctional?

Earlier, I said commands are similar to first-class functions or closures, but
every example I showed here used class definitions. If you're familiar with
functional programming, you're probably wondering where the functions are.

I wrote the examples this way because C++ has pretty limited support for
first-class functions. Function pointers are stateless, functors are weird and
still
require defining a class, and the lambdas in C++11 are tricky to work with
because of manual memory management.

That's *not* to say you shouldn't use functions for the Command pattern in other
languages. If you have the luxury of a language with real closures, by all means,
use them! In <span name="some">some</span> ways, the Command pattern is a way of
emulating closures in languages that don't have them.

<aside name="some">

I say *some* ways here because building actual classes or structures for
commands is still useful even in languages that have closures. If your command
has multiple operations (like undoable commands), mapping that to a single
function is awkward.

Defining an actual class with fields also helps readers easily tell what data
the command contains. Closures are a wonderfully terse way of automatically
wrapping up some state, but they can be so automatic that it's hard to see what
state they're actually holding.

</aside>

For example, if we were building a game in JavaScript, we could create a move
unit command just like this:

    :::javascript
    function makeMoveUnitCommand(unit, x, y) {
      // This function here is the command object:
      return function() {
        unit.moveTo(x, y);
      }
    }

We could add support for undo as well using a pair of closures:

    :::javascript
    function makeMoveUnitCommand(unit, x, y) {
      var xBefore, yBefore;
      return {
        execute: function() {
          xBefore = unit.x();
          yBefore = unit.y();
          unit.moveTo(x, y);
        },
        undo: function() {
          unit.moveTo(xBefore, yBefore);
        }
      };
    }

If you're comfortable with a functional style, this way of doing things is
natural. If you aren't, I hope this chapter helped you along the way a bit. For
me, the usefulness of the Command pattern really shows how effective the
functional paradigm is for many problems.

## See Also

 *  You may end up with a lot of different command classes. In order to make it
    easier to implement those, it's often helpful to define a concrete base
    class with a bunch of convenient high-level methods that the derived
    commands can compose to define their behavior. That turns the command's main
    `execute()` method into a <a href="subclass-sandbox.html"
    class="pattern">Subclass Sandbox</a>.

 *  In our examples, we explicitly chose which actor would handle a command. In
    some cases, especially where your object model is hierarchical, it may not
    be so cut-and-dried. An object may respond to a command, or it may decide to
    pawn it off on some subordinate object. If you do that, you've got yourself
    a <a class="gof-pattern" href="
    http://en.wikipedia.org/wiki/Chain-of-responsibility_pattern">Chain of Responsibility</a>.

 *  Some commands are stateless chunks of pure behavior like the `JumpCommand`
    in the first example. In cases like that, having <span
    name="singleton">more</span> than one instance of that class wastes memory
    since all instances are equivalent. The <a class="gof-pattern"
    href="flyweight.html">Flyweight</a> pattern addresses that.

    <aside name="singleton">

    You could make it a <a href="singleton.html" class="gof-
    pattern">Singleton</a> too, but friends don't let friends create singletons.

    </aside>
