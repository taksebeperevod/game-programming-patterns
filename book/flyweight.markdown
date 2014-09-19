^title Приспособленец
^section Пересмотренные паттерны проектирования

Поднимается туман, открывая взору старый величественный лес. Древние болиголовы
бесчисленным числом возвышаются над вами, образуя собор из зелени. Витражный
навес из листьев преломляет солнечный свет, превращая его в золотые лучи тумана.
Меж гигантских стволов можно разглядеть густой лес, простирающийся
вдаль.

О подобном другом мире мы мечтаем, будучи разработчиками игр, и
такие сцены часто возможны благодаря паттерну
со скромнейшим названием "Приспособленец" (Flyweight).

## Лес вместо деревьев

Я могу описать густой лес всего несколькими предложениями, но фактически
его *реализация* в игре в режиме реального времени -- совсем другая история. Когда целый
лес из отдельных деревьев заполняет экран, всё, что видит программист графики --
это миллионы полигонов, которые нужно как-то передавать видеокарте
каждую шестидесятую часть секунды.

Мы говорим о тысячах деревьев, каждое из которых имеет детализированную геометрию
из тысяч полигонов. Даже если у вас хватает *памяти*, чтобы описать этот лес,
чтобы отрендерить его, данные должны проделать путь по шине от центрального процессора
до видеокарты.

Каждое дерево имеет набор связанных с ним данных:

* Сетка из полигонов, которые определяют форму ствола, ветвей и зелени.
* Текстуры для коры и листьев.
* Его расположение и ориентация в лесу.
* Настраиваемые параметры, вроде размера и оттенка, чтобы каждое дерево выглядело разным.

Если бы пришлось набросать перечисленное в коде, вышло бы что-то вроде этого:

^code heavy-tree

Тут много данных, и особенно велики сетка и текстуры. Целый
лес данных объектов -- это слишком много, чтобы выбрасывать на центральный процессор за один кадр.
К счастью, есть проверенный временем трюк, позволяющий решить это проблему.

Ключевое наблюдение заключается в том, что хотя в лесу могут быть
тысячи деревьев, в основном все они выглядят похоже. Скорее всего, все они используют <span
name="same">одинаковую</span> сетку и текстуры. Это значит, что большинство полей в
данных объектах *одинаковы* во всех экземплярах.

<aside name="same">

Нужно быть сумасшедшим или миллиардером с соответствующим бюджетом для художников, чтобы
индивидуально моделировать каждое дерево в целом лесу.

</aside>

<span name="trees"></span>

<img src="images/flyweight-trees.png" alt="Ряд деревьев, каждое из которых имеет собственную сетку, кору, листья, параметры и положение." />

<aside name="trees">

Обратите внимание, что написанное в маленьких прямоугольниках одинаково для каждого дерева.

</aside>

Можно смоделировать это явно, разбив объект наполовину. Сперва вытащим
данные, являющиеся <span name="type">общими</span> для всех деревьев и переместим их
в отдельный класс:

^code tree-model

Игре необходим только один из них, потому что нет причины помещать
одни и те же сетки и текстуры в память тысячу раз. Затем каждый *экземпляр*
дерева в мире получает *ссылку* на совместно используемый `TreeModel`. Что остается в
`Tree` -- состояние, уникальное для каждого экземпляра:

^code split-tree

Можно представить это так:

<img src="images/flyweight-tree-model.png" alt="Ряд деревьев, где каждое имеет собственные параметры и позицию, но указывает на совместно используемую модель с сеткой, корой и листьями." />

<aside name="type">

Выглядит очень похоже на паттерн <a href="type-object.html" class="pattern">"Тип
объекта"</a>. Оба включают делегирование части состояния объекта некоему
другому объекту, совместно используемому несколькими экземплярами. Как бы то ни было, цели
паттернов различны.

Цель паттерна "Тип объекта" (Type Object) -- минимизировать количество
классов, требующих определения, путем проталкивания "типов" в собственную объектную модель. Любое совместное использование памяти, получаемое при этом,
всего лишь бонус. Паттерн "Приспособленец" (Flyweight) полностью нацелен на эффективность.

</aside>

Все это хорошо для хранения в основной памяти, но не
помогает при рендеринге. Перед тем как лес окажется на экране, ему нужно пройти путь до
центрального процессора. Мы должны выразить это совместное использование ресурсов способом,
понятным видеокарте.

## Тысяча экземпляров

Чтобы минимизировать количество данных, которые нужно протолкнуть к видеокарте, хочется иметь возможность
отослать совместно используемые данные -- `TreeModel` -- только *однажды*. Затем отдельно
проталкиваем уникальные данные каждого экземпляра дерева -- его позицию, цвет и масштаб.
Наконец, говорим видеокарте: "Используй ту модель, чтобы отрендерить каждый из этих
экземпляров."

К счастью, сегодняшние графические API и <span name="hardware">карты</span>
поддерживают именно это. Подробности утомительны и выходят за рамки данной книги,
но как Direct3D, так и OpenGL могут осуществлять нечто, называемое [*рендерингом
инстансингом*](http://ru.wikipedia.org/wiki/Geometry_Instancing).

В обоих API вы предоставляете два потока данных. Первый является двоичным объектом общих
данных, которые будут рендериться несколько раз -- сетка и текстуры в нашем
"древесном" примере. Второй -- это список экземпляров и их параметров, которые
будут использоваться для того, чтобы разнообразить первый кусок данных каждый раз при отрисовке. С помощью
единственного вызова метода отрисовки появляется целый лес.

<aside name="hardware">

Тот факт, что это API реализуется напрямую графической картой, означает, что
паттерн "Приспособленец", может быть, единственный паттерн проектирования "Банды четырех", имеющий реальную
аппаратную поддержку.

</aside>

## Паттерн "Приспособленец"

Теперь, когда в нашем запасе есть один конкретный пример, я могу провести вас
в целом по паттерну. "Приспособленец" ("Flyweight" по англ. -- игра слов "fly" -- лететь, "weight" -- вес) вступает в игру, когда есть
объекты, требующие большей "легковесности", как правило, потому что их слишком
много.

При рендеринге инстансингом они не столько едят память,
сколько отнимают *время* на проталкивание каждого отдельного дерева по шине к
видеокарте, но основная идея остается той же.

Паттерн решает это, разделяя данные объекта на два вида.
Первый вид данных -- это не уникальные для отдельного *экземпляра*
данного объекта вещи, которые могут совместно использоваться всеми экземплярами. "Банда четырех" называет их
*внутренним* состоянием, но мне нравится думать о них, как о "контекстно-свободных" вещах. В
приведенном примере, это геометрия и текстуры для дерева.

Остальные данные -- это *внешнее* состояние, вещи, уникальные для данного
экземпляра. В данном случае это позиция, масштаб и цвет каждого дерева. Прямо
как в куске кода из приведенного примера, данный паттерн экономит память, позволяя совместно использовать
одну копию внешнего состояния всюду, где появляется объект.

Исходя из ранее увиденного, это кажется обычным совместным использованием ресурсов
и вряд ли стоит того, чтобы называться паттерном. Отчасти так происходит потому, что в приведенном примере
мы могли бы создать совершенно отдельную *сущность* для совместно используемого состояния:
`TreeModel`.

Я нахожу этот паттерн менее очевидным (и поэтому, более хитрым) при использовании в случаях,
когда нет действительно хорошо определенной сущности для совместно используемого объекта. В таких
случаях кажется, что объект магическим образом находится в нескольких местах
одновременно. Позвольте продемонстрировать другой пример.

## A Place To Put Down Roots

The ground these trees are growing on needs to be represented in our game too.
There can be patches of grass, dirt, hills, lakes, rivers, and whatever other
terrain you can dream up. We'll make the ground *tile-based*: the surface of the
world is a huge grid of tiny tiles. Each tile is covered in one kind of terrain.

Each terrain type has a number of properties that affect gameplay:

* A movement cost that determines how quickly players can move through it.
* A flag for whether it's a watery terrain that can be crossed by boats.
* A texture used to render it.

Because we game programmers are paranoid about efficiency, there's no way we'd
store all of that state in <span name="learned">each</span> tile in the world.
Instead, a common approach is to use an enum for terrain types:

<aside name="learned">

After all, we already learned our lesson with those trees.

</aside>

^code terrain-enum

Then the world maintains a huge grid of those:

<span name="grid"></span>

^code enum-world

<aside name="grid">

Here I'm using a nested array to store the 2D grid. That's efficient in C/C++
because it will pack all of the elements together. In Java or other memory-
managed languages, doing that will actually give you an array of rows where each
element is a *reference* to the array of columns, which may not be as memory-
friendly as you'd like.

In either case, real code would be better served by hiding this implementation
detail behind a nice 2D grid data structure. I'm doing this here just to keep it
simple.

</aside>

To actually get the useful data about a tile, we do something like:

^code enum-data

You get the idea. This works, but I find it ugly. I think of movement cost and
wetness as *data* about a terrain, but here that's embedded in code. Worse, the
data for a single terrain type is smeared across a bunch of methods. It would be
really nice to keep all of that encapsulated together. After all, that's what
objects are designed for.

It would be great if we could have an actual terrain *class*, like:

<span name="const"></span>

^code terrain-class

<aside name="const">

You'll notice that all of the methods here are `const`. That's no coincidence.
Since the same object is used in multiple contexts, if you were to modify it,
the changes would appear in multiple places simultaneously.

That's probably not what you want. Sharing objects to save memory should be an
optimization that doesn't affect the visible behavior of the app. Because of
this, Flyweight objects are almost always immutable.

</aside>

But we don't want to pay the cost of having an instance of that for each tile in
the world. If you look at that class, you'll notice that there's actually
*nothing* in there that's specific to *where* that tile is. In flyweight terms,
*all* of a terrain's state is "intrinsic" or "context-free".

Given that, there's no reason to have more than one of each terrain type. Every
grass tile on the ground is identical to every other one. Instead of having the
world be a grid of enums or Terrain objects, it will be a grid of *pointers* to
`Terrain` objects:

^code world-terrain-pointers

Each tile that uses the same terrain will point to the same terrain instance.

<img src="images/flyweight-tiles.png" alt="A row of tiles. Each tile points to either a shared Grass, River, or Hill object." />

Since the terrain instances are used in multiple places, their lifetimes would
be a little more complex to manage if you were to dynamically allocate them.
Instead, we'll just store them directly in the world:

^code world-terrain

Then we could use those to paint the ground like this:

<span name="generate"></span>

^code generate

<aside name="generate">

I'll admit this isn't the world's greatest procedural terrain generation
algorithm.

</aside>

Now instead of methods on `World` for accessing the terrain properties, we can
expose the `Terrain` object directly:

^code get-tile

This way, `World` is no longer coupled to all sorts of details of terrains. If
you want some property of the tile, you can get it right from that object:

^code use-get-tile

We're back to the pleasant API of working with real objects, and we did this
with almost no overhead -- a pointer is often no larger than an enum.

## What About Performance?

I say "almost" here because the performance bean counters will rightfully want
to know how this compares to using an enum. Referencing the terrain by pointer
implies an indirect lookup. To get to some terrain data like the movement cost,
you first have to follow the pointer in the grid to find the terrain object and
then find the movement cost there. Chasing a pointer like this can cause a <span
name="cache">cache miss</span>, which can slow things down.

<aside name="cache">

For lots more on pointer chasing and cache misses, see the chapter on <a href
="data-locality.html" class="pattern">Data Locality</a>.

</aside>

As always, the golden rule of optimization is *profile first*. Modern computer
hardware is too complex for performance to be a game of pure reason anymore. In
my tests for this chapter, there was no penalty for using a flyweight over an
enum. Flyweights were actually noticeably faster. But that's entirely dependent
on how other stuff is laid out in memory.

What I *am* confident of is that using flyweight objects shouldn't be dismissed
out of hand. They give you the advantages of an object-oriented style without
the expense of tons of objects. If you find yourself creating an enum and doing
lots of switches on it, consider this pattern instead. If you're worried about
performance, at least profile first before changing your code to a less
maintainable style.

## See Also

 *  In the tile example, we just eagerly created an instance for each terrain
    type and stored it in `World`. That made it easy to find and reuse the
    shared instances. In many cases, though, you won't want to create *all* of
    the flyweights up front.

    If you can't predict which ones you actually need, it's better to create
    them on demand. To get the advantage of sharing, when you request one, you
    first see if you've already created an identical one. If so, you just return
    that instance.

    This usually means that you have to encapsulate construction behind some
    interface that can first look for an existing object. Hiding a constructor
    like this is an example of the <a
    href="http://en.wikipedia.org/wiki/Factory_method_pattern" class="gof-
    pattern">Factory Method</a> pattern.

    In order to return a previously created flyweight, you'll have to keep track
    of the pool of ones that you've already instantiated. As the name implies,
    that means that an <a href="object-pool.html" class="pattern">Object
    Pool</a> might be a helpful place to store them.

 *  When you're using the <a class="pattern" href="state.html">State</a>
    pattern, you often have "state" objects that don't have any fields specific
    to the machine that the state is being used in. The state's
    identity and methods are enough to be useful. In that case, you can apply
    this pattern and reuse that same state instance in multiple state machines
    at the same time without any problems.
