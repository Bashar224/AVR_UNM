El presente proyecto fue realizado en el marco del curso "Programación y Simulación
de Microcontroladores de ocho bits", de la Universidad Nacional de Moreno. Este 
proyecto fue realizado por los alumnos:
                                        - Clos, Ana María
                                        - López Lombardo, Ignacio Nahuel
                                        - Prior, Xoana Vanesa
                                        - Salinas, Erika Denise

- INTRODUCCIÓN:

El proyecto consiste en un control de ingreso de usuarios. Mediante el uso de un 
teclado matricial se ingresa un nombre de usuario y contraseña. Si estos datos son
válidos el acceso es concedido, en caso contrario el acceso es denegado.

- DESCRIPCIÓN:

En la memoria del microcontrolador se almacenan varios pares de usuarios con sus 
respectivas contraseñas. El usuario y la contraseña están formados por cuatro 
caracteres numéricos cada uno y están separados entre si por un asterisco.

El operador ingresa un número de usuario, presiona asterisco y luego ingresa la 
contraseña, el ingreso finaliza con otro asterisco. Para borrar la contraseña ingresada
se oprime numeral, y presionando nuevamente esta tecla (#) se reinicia el texto.

El programa recorre la memoria buscando, entre los pares almacenados, si se encuentra 
el ingresado. Si el usuario es válido, se enciende un led verde y si no lo es, uno rojo.
En ambos casos se muestra en la terminal virtual la cadena ingresada y una frase indicando
si el acceso está concedido o denegado.

- COMPONENTE CENTRAL:

El proyecto se desarrolla en un microcontrolador ATmega48, con una frecuencia de clock
de 8MHz. El teclado matricial se conecta el puerto B y éste se configura para detectar
interrupciones externas. Se establece una conexión serial, para transmitir datos a la 
terminal virtual. Se establece un baudrate de 19200, con 8 bits de datos y sin bit de paridad.
