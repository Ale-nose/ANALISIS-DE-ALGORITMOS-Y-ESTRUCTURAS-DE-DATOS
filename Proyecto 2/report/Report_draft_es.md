# Maze Craze: Reporte Experimental

## Metodología

Todas las mediciones provienen de la instrumentación del propio proyecto.
`GenerationMetrics` y `SearchMetrics` se llenan dentro de cada algoritmo, y el
tiempo de CPU se toma con `std::clock` alrededor del algoritmo únicamente, nunca
alrededor de la interfaz, de modo que las cifras excluyen el renderizado y la
entrada del usuario. Todas las herramientas se compilaron con `-O2`, así que los
tiempos describen el comportamiento de los algoritmos en modo release.

Se utilizan tres conjuntos de datos:

- `data/maze_comparison.csv`: 120 corridas producidas por `tools/MazeAnalysis.cpp`.
  Dos generadores (DFS y BFS), cuatro tamaños cuadrados (10x10, 20x20, 30x30 y
  40x40), tres valores de `EXTRA_EDGE_PROB` (0.0, 0.2 y 0.5) y cinco semillas
  (42, 100, 314, 7 y 2026). Ambos generadores reciben la misma semilla en cada
  combinación, por lo que la comparación es pareada.
- `data/path_comparison.csv`: 180 corridas producidas por `tools/PathComparison.cpp`.
  Tres algoritmos (BFS, Dijkstra y A\*) sobre los mismos laberintos, con los
  mismos cuatro tamaños, tasas de barro de 0.0, 0.2 y 0.5 y las mismas cinco
  semillas. Los tres algoritmos corren siempre sobre un laberinto idéntico, así
  que cualquier diferencia proviene del algoritmo y no del mapa.
- `data/benchmark_results.csv`: 36 partidas completas producidas por
  `tools/BenchmarkRunner.cpp`. Tres tamaños (10x20, 15x30 y 20x40), tres tasas de
  barro (0.05, 0.12 y 0.25), ambos generadores y dos repeticiones de cada
  combinación, con las semillas listadas en `data/seeds.txt`. A diferencia de los
  otros dos conjuntos, estas filas provienen de partidas jugadas hasta el final y
  no de llamadas aisladas a los algoritmos. Ambos jugadores son automáticos: el
  jugador 1 sigue la ruta A\* más barata hacia el tesoro, y el jugador 2 es el
  mismo `AIController` que usa el juego, con sus Jump Walls gastados al azar y
  sus búsquedas registradas en el motor. Correr la herramienta dos veces produce
  el mismo archivo salvo por los tiempos de CPU, de modo que toda partida es
  reproducible a partir de su semilla.

Los cuatro tamaños dan 100, 400, 900 y 1600 celdas, un rango de 16x lo bastante
amplio para observar cómo crece el costo de cada algoritmo. Las celdas de barro
cuestan `MUD_COST = 2` al entrar y las celdas normales cuestan 1. El costo de una
ruta es la suma del costo de entrada de cada celda a la que entra, sin cobrar la
celda inicial.

Ambas herramientas miden la ruta entre las dos esquinas opuestas del laberinto,
de (0, 0) a (rows - 1, cols - 1), y no entre las filas iniciales aleatorias que
produce `chooseStartPositions` durante una partida real. Los extremos fijos
mantienen constante la distancia entre los puntos para un tamaño dado, así que
las diferencias entre las filas provienen solo del algoritmo y del terreno.

## Generación del Laberinto

### DFS contra BFS

| Prob. de arista extra | Algoritmo | Callejones sin salida | Costo de ruta | Aristas extra | CPU (ms) |
|---|---|---|---|---|---|
| 0.0 | DFS | 40.0 | 149.6 | 0 | 0.056 |
| 0.0 | BFS | 20.0 | 38.0 | 0 | 0.025 |
| 0.2 | DFS | 20.6 | 44.0 | 73.4 | 0.058 |
| 0.2 | BFS | 13.4 | 38.0 | 71.0 | 0.024 |
| 0.5 | DFS | 7.4 | 38.0 | 173.0 | 0.058 |
| 0.5 | BFS | 5.0 | 38.0 | 176.0 | 0.027 |

Laberintos de 20x20, promedios sobre las cinco semillas.

Ambos generadores exploran todas las celdas del laberinto y abren exactamente
V - 1 pasajes antes de los extra, lo que confirma que cada uno produce un árbol
de expansión: el `createdEdges` medido equivale a `V - 1 + extraEdges` en las 120
corridas.

Los dos generadores difieren marcadamente en la forma del laberinto. Sin pasajes
extra, el laberinto DFS obliga a una ruta de 149.6 celdas en promedio entre las
dos esquinas, casi cuatro veces las 38 del laberinto BFS, y deja el doble de
callejones sin salida (40.0 contra 20.0). Esto coincide con lo que hacen los
algoritmos: el recorrido en profundidad sigue un corredor hasta donde llegue y
solo entonces retrocede, lo que produce corredores largos y sinuosos con muchas
ramas que no llevan a ningún lado, mientras que el recorrido en anchura se
expande por capas, de modo que la ruta que deja entre dos celdas es la más corta
que la cuadrícula permite.

El resultado de BFS es de hecho exacto, no aproximado. En las 20 corridas del
conjunto de datos su costo de ruta es siempre 2(n - 1), la distancia Manhattan
entre las dos esquinas: 18, 38, 58 y 78 para los cuatro tamaños. Un árbol
construido por búsqueda en anchura conserva la distancia de cada celda a la raíz,
así que la ruta de esquina a esquina nunca puede ser más larga que la distancia
en la cuadrícula.

`EXTRA_EDGE_PROB` borra esa diferencia. En 0.2 el costo de ruta de DFS cae de
149.6 a 44.0, y en 0.5 ambos generadores dan exactamente 38, el mínimo posible.
Los callejones sin salida también disminuyen en ambos casos, de 40.0 a 7.4 en DFS
y de 20.0 a 5.0 en BFS, porque cada pasaje extra puede darle un segundo vecino a
una celda que solo tenía uno. En otras palabras, el carácter del generador solo
es visible mientras el laberinto se mantiene cerca de un árbol: una vez que se
agregan suficientes ciclos, ambos convergen a la misma cuadrícula.

### El efecto del tamaño del mapa

| Tamaño | Celdas | Callejones DFS | Callejones BFS | Ruta DFS | Ruta BFS |
|---|---|---|---|---|---|
| 10x10 | 100 | 11.2 | 10 | 43.2 | 18 |
| 20x20 | 400 | 40.0 | 20 | 149.6 | 38 |
| 30x30 | 900 | 92.8 | 30 | 322.0 | 58 |
| 40x40 | 1600 | 162.0 | 40 | 410.0 | 78 |

Árboles de expansión sin pasajes extra, promedios sobre las cinco semillas.

Los dos generadores escalan de maneras distintas. DFS deja una fracción constante
del laberinto como callejones sin salida, cercana al 10% de las celdas en todos
los tamaños (11.2 de 100, 40.0 de 400, 92.8 de 900 y 162.0 de 1600). BFS deja
exactamente n callejones sin salida en un laberinto de n x n, en las 20 corridas
y con todas las semillas, así que su fracción cae como 1/n: 10% de un laberinto
de 10x10 pero solo 2.5% de uno de 40x40.

La brecha en la longitud de la ruta se ensancha con el tamaño. La ruta DFS es 2.4
veces la distancia Manhattan en un laberinto de 10x10 y más de 5 veces en 30x30 y
40x40, de modo que un laberinto en profundidad no solo serpentea más, sino que
serpentea desproporcionadamente más conforme crece el mapa. Las corridas
individuales también se dispersan ampliamente, de 234 a 590 celdas en 40x40,
porque una sola decisión aleatoria al inicio del recorrido cambia el corredor
completo. La ruta BFS, en cambio, da el mismo valor exacto para cada semilla.

## Comparación de Algoritmos de Búsqueda

| Tasa de barro | Algoritmo | Nodos visitados | Costo de ruta | CPU (ms) |
|---|---|---|---|---|
| 0.0 | BFS | 388.2 | 44.0 | 0.061 |
| 0.0 | Dijkstra | 390.2 | 44.0 | 0.087 |
| 0.0 | A\* | 236.4 | 44.0 | 0.047 |
| 0.2 | BFS | 388.2 | 52.8 | 0.058 |
| 0.2 | Dijkstra | 388.4 | 50.4 | 0.081 |
| 0.2 | A\* | 270.8 | 50.4 | 0.059 |
| 0.5 | BFS | 388.2 | 67.8 | 0.057 |
| 0.5 | Dijkstra | 389.8 | 65.4 | 0.080 |
| 0.5 | A\* | 329.0 | 65.4 | 0.064 |

Laberintos de 20x20, promedios sobre las cinco semillas.

### BFS no minimiza el costo

Sin barro los tres algoritmos reportan el mismo costo en las 20 corridas del
conjunto de datos, porque todas las celdas cuestan lo mismo y la ruta más barata
es también la más corta. En cuanto aparece el barro se separan: BFS devuelve una
ruta más cara en 27 de las 40 corridas con barro, y nunca una más barata.
Dijkstra y A\* coinciden en el costo en las 180 corridas.

La razón es que BFS cuenta pasos y no costos. Expande celdas en orden de
distancia, así que devuelve una ruta con la menor cantidad de celdas, y el costo
real se calcula solo después. Siempre que una ruta más corta cruza barro y una
más larga lo evita, BFS se queda con la cara.

El caso reproducible más claro es la semilla 100 en un laberinto de 30x30 con una
tasa de barro de 0.2:

| Algoritmo | Pasos | Costo de ruta |
|---|---|---|
| BFS | 62 | 76 |
| Dijkstra | 66 | 71 |
| A\* | 66 | 71 |

BFS llega a la meta en cuatro pasos menos y aun así paga 7% más, porque las
cuatro celdas extra de la otra ruta son más baratas que el barro que evita. La
semilla 314 en un laberinto de 20x20 con la misma tasa de barro muestra el mismo
efecto con un margen menor: 48 pasos costando 59 contra 50 pasos costando 58.

En promedio la penalización es moderada, entre 2.9% y 4.9% según el tamaño y la
tasa de barro, pero mapas individuales alcanzan 15.2% (semilla 42, 20x20, barro
0.2: 53 contra 46). El efecto además necesita espacio para aparecer: en
laberintos de 10x10 BFS nunca pierde, porque con tan pocas celdas y
`EXTRA_EDGE_PROB` en 0.2 la ruta más corta suele ser la única, y no hay ruta
alternativa que pueda resultar más barata.

### A\* abre menos celdas

A\* encuentra el mismo costo que Dijkstra en todas las corridas, que es lo que
garantiza la heurística Manhattan: cada paso cuesta al menos 1, no hay atajos en
estas búsquedas, así que la heurística nunca sobreestima el costo restante y la
primera ruta que cierra es óptima.

Su ganancia está en la cantidad de celdas que tiene que abrir. Agrupando las tres
tasas de barro visita entre 62% y 72% de lo que visita Dijkstra en los cuatro
tamaños, porque la heurística empuja la búsqueda hacia la meta en lugar de
expandirse en todas las direcciones. Dijkstra y BFS, en cambio, abren casi todo
el laberinto: 893 de 900 celdas en 30x30 y 1586 de 1600 en 40x40, lo cual es
esperable cuando la meta está en la esquina opuesta y ninguna parte del laberinto
puede descartarse.

La ventaja de A\* se reduce conforme crece el barro, y lo hace en todos los
tamaños:

| Tamaño | barro 0.0 | barro 0.2 | barro 0.5 |
|---|---|---|---|
| 10x10 | 60.6% | 61.2% | 74.6% |
| 20x20 | 59.1% | 67.7% | 82.3% |
| 30x30 | 46.3% | 58.8% | 78.8% |
| 40x40 | 49.4% | 62.0% | 81.1% |

Celdas abiertas por A\*, como porcentaje del laberinto.

La heurística sigue siendo admisible, pero se vuelve menos informativa: asume un
costo de 1 por paso mientras muchas celdas ahora cuestan 2, así que subestima el
costo restante por un margen más amplio y descarta menos ramas. Entre más barro
haya, más se comporta A\* como Dijkstra, y con una tasa de barro de 0.5 ya abre
cerca del 80% del laberinto.

## Partidas Completas

Las 36 partidas de `data/benchmark_results.csv` confirman, sobre partidas jugadas
hasta el final, el comportamiento que las mediciones aisladas ya mostraban.

| Tamaño | Generador | Costo de ruta | Turnos | CPU de generación (ms) |
|---|---|---|---|---|
| 10x20 | DFS | 44.5 | 36.0 | 0.044 |
| 10x20 | BFS | 24.7 | 24.5 | 0.013 |
| 15x30 | DFS | 76.8 | 59.8 | 0.096 |
| 15x30 | BFS | 38.5 | 32.5 | 0.035 |
| 20x40 | DFS | 86.0 | 75.2 | 0.173 |
| 20x40 | BFS | 48.2 | 54.8 | 0.059 |

Promedios sobre las seis partidas de cada combinación.

El generador decide cuánto dura una partida. La ruta entre los dos jugadores
cuesta alrededor del doble en un laberinto DFS que en uno BFS en los tres
tamaños, y las partidas duran 1.53 veces más en promedio, 57.0 turnos contra
37.3. Esta es la misma diferencia en longitud de corredor que mide la sección de
generación, ahora expresada en el tiempo que un jugador pasa en el tablero. La
razón aquí es menor que la de los árboles de expansión, alrededor de cuatro
veces, porque estas partidas usan el `EXTRA_EDGE_PROB` por defecto, cuyos atajos
acortan el laberinto DFS.

El barro eleva el costo de la ruta como se esperaba, y lo hace para ambos
generadores:

| Tasa de barro | Costo de ruta DFS | Costo de ruta BFS |
|---|---|---|
| 0.05 | 62.5 | 35.2 |
| 0.12 | 68.3 | 37.2 |
| 0.25 | 76.5 | 39.0 |

Promedios sobre las seis partidas de cada combinación, todos los tamaños
agrupados.

De 0.05 a 0.25 el costo crece 22.4% en laberintos DFS y 10.8% en los BFS. La
diferencia entre ambos es consistente con su forma: una ruta DFS visita más
celdas, así que tiene más oportunidades de pisar barro.

### Lo que estas filas no miden

Hacen falta tres observaciones para leer la tabla correctamente.

La columna `visited` contiene exactamente `rows * cols` en las 36 filas, lo que
significa que cuenta las celdas exploradas durante la generación del laberinto y
no durante alguna búsqueda. Toda generación visita todas las celdas, así que esta
columna es constante para un tamaño dado y no agrega información más allá de la
que dan los otros dos conjuntos de datos.

Los tiempos de CPU individuales se dispersan ampliamente alrededor de su
promedio, de 0.098 a 0.305 ms entre las seis partidas de 20x40 con DFS. Se suman
dos causas. Los tiempos ya son lo bastante pequeños, decenas de microsegundos,
para que la resolución del reloj y el planificador pesen en cada medición, y las
primeras partidas de una corrida son sistemáticamente más lentas que las últimas,
que es el calentamiento usual del procesador. Los promedios de la tabla absorben
ambos efectos, pero una corrida futura que repitiera cada medición y se quedara
con el mínimo daría cifras más limpias.

Dos partidas tienen una cantidad de turnos muy por encima de sus vecinas: 118
turnos en el laberinto de 20x40 de la semilla 2042, donde las otras cinco
partidas de esa combinación duran entre 38 y 46, y 42 turnos en el laberinto de
10x20 de la semilla 2037, contra 18 a 23. Sus costos de ruta son ordinarios, así
que la duración viene de cómo se desarrolló la partida y no del mapa.

### El balance de las partidas

El jugador 1 gana 21 de las 36 partidas y el jugador 2 gana las 15 restantes,
58.3% contra 41.7%. Los dos jugadores siguen la misma estrategia, así que no se
esperaba una diferencia, pero dos asimetrías favorecen al jugador 1: se mueve
primero, y el tesoro se coloca a la mitad del costo de la ruta que une a ambos
jugadores, de modo que en una carrera donde ambos siguen una ruta óptima el que
empieza llega primero. El jugador 2 además gasta sus Jump Walls al azar, lo que
puede ahorrarle turnos o desperdiciarlos.

La desviación es, sin embargo, demasiado pequeña para afirmar algo. Con 36
partidas y un juego balanceado el resultado esperado es 18 victorias con una
desviación estándar de 3, así que 21 está a una desviación estándar y es
perfectamente compatible con el azar.

Separar por generador da una imagen más sugerente, y aplica la misma cautela:

| Generador | Jugador 1 | Jugador 2 | Turnos promedio |
|---|---|---|---|
| DFS | 9 | 9 | 57.0 |
| BFS | 12 | 6 | 37.3 |

En las partidas largas de los laberintos DFS el resultado es un empate exacto,
mientras que en las partidas cortas de los laberintos BFS el jugador 1 gana el
doble de veces. Una lectura plausible es que la ventaja de mover primero pesa más
cuando la partida dura 37 turnos que cuando dura 57, porque las decisiones
aleatorias del jugador 2 tienen menos turnos en los cuales cambiar el desenlace.
Con solo 18 partidas por generador esto es una observación y no un resultado: la
desviación es de 1.4 desviaciones estándar, que el azar produce aproximadamente
una vez de cada seis. Confirmarlo requeriría unos cientos de partidas, que la
herramienta ya puede generar extendiendo la lista de semillas.

## Complejidad

BFS corre en O(V + E): cada celda entra y sale una vez de una cola FIFO, y cada
pasaje se examina una vez. Dijkstra con un heap binario corre en O((V + E) log V),
porque cada una de las V extracciones y cada una de las E mejoras posibles cuesta
una operación logarítmica sobre el heap. A\* tiene la misma cota de peor caso que
Dijkstra, puesto que es Dijkstra con las prioridades desplazadas por la
heurística; lo que la heurística cambia es la constante y la cantidad de celdas
realmente abiertas, no el orden asintótico.

### Crecimiento medido

| Celdas (V) | BFS | Dijkstra | A\* | Gen. DFS | Gen. BFS |
|---|---|---|---|---|---|
| 100 | 0.0151 | 0.0200 | 0.0137 | 0.0166 | 0.0072 |
| 400 | 0.0586 | 0.0826 | 0.0568 | 0.0572 | 0.0251 |
| 900 | 0.1284 | 0.1917 | 0.1095 | 0.1250 | 0.0521 |
| 1600 | 0.2342 | 0.3345 | 0.2111 | 0.2225 | 0.0909 |

Tiempo de CPU en milisegundos, promediado sobre semillas y sobre tasas de barro o
probabilidades de arista extra. Se removieron los tres valores atípicos más
lentos del conjunto de datos, como se explica abajo.

La forma más clara de leer la tabla es el tiempo por celda. Para BFS es 0.000151,
0.000147, 0.000143 y 0.000146 ms en los cuatro tamaños: esencialmente constante
mientras V crece 16 veces. Lo mismo vale para ambos generadores, 0.000166 a
0.000139 ms por celda para DFS y 0.000072 a 0.000057 para BFS. Un costo por celda
que no depende del tamaño del laberinto es exactamente lo que predice O(V + E), y
en una cuadrícula E es a su vez proporcional a V, así que los dos términos crecen
juntos.

Dijkstra se comporta distinto, como era de esperarse. Su tiempo por celda es
0.000200, 0.000207, 0.000213 y 0.000209 ms, un incremento lento en lugar de una
línea plana, y su tiempo total crece por un factor de 16.7 mientras V crece 16.
El crecimiento extra es el factor log V del heap. Es visible pero mucho más débil
de lo que la teoría por sí sola sugeriría: log2(V) crece de 6.6 a 10.6 en este
rango, un factor de 1.6, mientras que la razón medida contra BFS solo se mueve de
1.33 a 1.43. La razón es que las operaciones del heap son una parte pequeña del
trabajo total en grafos de este tamaño, y los factores constantes de las dos
implementaciones dominan la medición.

A\* se ubica entre los dos. Su orden asintótico es el de Dijkstra, pero el tiempo
por celda se mantiene cerca del de BFS, entre 0.000122 y 0.000142 ms, porque abre
menos celdas y paga el costo logarítmico sobre una frontera más pequeña. En los
mapas más grandes esto lo convierte en el más rápido de los tres a pesar del
heap, con 0.2111 ms contra 0.2342 de BFS y 0.3345 de Dijkstra.

### Efecto de la tasa de barro

El barro no cambia el costo asintótico de ningún algoritmo, y las mediciones lo
confirman: en 20x20 las tres tasas de barro dan 0.061, 0.058 y 0.057 ms para BFS,
diferencias dentro del ruido de la medición. El barro cambia el costo de las
rutas y la cantidad de celdas que abre A\*, como muestra la sección anterior,
pero no la cantidad de operaciones que realizan los algoritmos, puesto que el
costo de entrada se lee de la celda y no agrega trabajo al recorrido.

### Valores atípicos

Tres mediciones del conjunto de datos están un orden de magnitud por encima de
sus vecinas: A\* en 0.614 ms y Dijkstra en 0.468 ms en el laberinto de 30x30 de
la semilla 7 con barro 0.5, y BFS en 0.529 ms en el laberinto de 40x40 de la
semilla 2026 con barro 0.5. Sus conteos de nodos visitados son normales, así que
la causa es el planificador del sistema operativo y no el algoritmo. Se
excluyeron de la tabla de crecimiento y se conservaron en el CSV. Repetir cada
medición varias veces y tomar el mínimo eliminaría este ruido en una corrida
futura.

### Una confirmación independiente

Las 36 partidas completas miden la generación en tres tamaños que ninguna de las
otras herramientas usa, así que sirven como verificación independiente de la
tabla anterior. Su tiempo por celda es 0.000220, 0.000213 y 0.000216 ms para DFS
en 200, 450 y 800 celdas, y 0.000067, 0.000078 y 0.000074 ms para BFS. La
constancia es la misma que muestra el otro conjunto de datos, y las cifras de BFS
concuerdan con los 0.000057 a 0.000072 ms medidos ahí. Las de DFS son algo más
altas, lo cual es esperable dado que estos laberintos usan el `EXTRA_EDGE_PROB`
por defecto y una relación de aspecto distinta, pero se mantienen en el mismo
orden de magnitud. Tres herramientas compiladas por separado, sobre siete tamaños
de mapa, dan por lo tanto la misma respuesta: el costo de generar un laberinto es
proporcional a la cantidad de celdas.

## Conclusiones

El generador decide la forma del laberinto mucho más que su costo. DFS y BFS
exploran las mismas celdas y abren los mismos V - 1 pasajes, pero el laberinto
DFS es casi cuatro veces más largo entre sus esquinas en 20x20 y más de cinco
veces en 40x40, y mantiene alrededor del 10% de sus celdas como callejones sin
salida en todos los tamaños mientras que BFS mantiene exactamente uno por fila.
Esa diferencia solo sobrevive mientras el laberinto se mantiene cerca de un
árbol: con `EXTRA_EDGE_PROB` en 0.5 ambos generadores dan la misma ruta mínima y
una cantidad similar de callejones sin salida, así que para el jugador la
elección de generador solo importa con probabilidades bajas.

Entre los algoritmos de búsqueda la elección depende de qué se esté optimizando.
BFS es el más simple y tiene la constante más baja, pero minimiza pasos y no
costo, así que con barro devolvió una ruta más cara en 27 de las 40 corridas,
hasta 15% más en un mapa individual. Dijkstra y A\* siempre coinciden en el costo
mínimo, y A\* lo alcanza abriendo entre 28% y 38% menos celdas. Ese ahorro
disminuye conforme aumenta el barro, porque la heurística Manhattan asume un
costo uniforme y pierde precisión, hasta que con una tasa de barro de 0.5 A\* ya
abre cerca del 80% del laberinto.

Los tiempos medidos siguen la teoría en el orden correcto. El tiempo por celda es
constante para BFS y para ambos generadores, como predice O(V + E), y crece
lentamente para Dijkstra, como predice el factor log V del heap, aunque el efecto
es menor de lo que la expresión asintótica sugiere en estos tamaños. Las 36
partidas completas reproducen el mismo costo constante por celda en tres tamaños
que las otras herramientas no usan.

Las partidas también muestran lo que todo esto significa para el jugador. Un
laberinto DFS hace que la ruta entre los dos jugadores cueste el doble y que la
partida dure 1.53 veces más que en uno BFS, y subir la tasa de barro de 0.05 a
0.25 agrega otro 22% a esa ruta. El generador y el barro son, por lo tanto, los
dos parámetros que más cambian la duración de una partida.

Para el juego esto justifica el diseño: el tesoro se coloca con Dijkstra, donde
el costo exacto importa y la búsqueda corre una vez por partida, y la IA
replanifica con A\*, donde la búsqueda se repite muchas veces y abrir menos
celdas importa más.