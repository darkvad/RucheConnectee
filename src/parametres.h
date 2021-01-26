/*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef parametres_h
#define parametres_h

#define STRING_LEN 128
#define NUMBER_LEN 32

char singleChannelGatewayValue[NUMBER_LEN];
char BMP280Value[NUMBER_LEN];
char nb_DS18B20Value[NUMBER_LEN];
char tempSensorsValues[3][STRING_LEN];
char sendIntervalValue[NUMBER_LEN];
char altitudeValue[NUMBER_LEN];
char objectIDValue[STRING_LEN];
char rucherValue[STRING_LEN];
char rucheValue[STRING_LEN];
char sleepValue[STRING_LEN];
char offsetValue[NUMBER_LEN];
char calibrationValue[NUMBER_LEN];

#endif