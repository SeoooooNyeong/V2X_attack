//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package simulation.ddosattack;
import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;
//import org.car2x.veins.nodes.TraCIDemo11p;

//import oddossimo.simulations.ddosattack.Car;

simple DDoSAttacker
{
    parameters:
        int attackInterval @unit(ms); // Interval between attacks
    gates:
        input in;
        output out[];
}

network V2XNetwork
{
    submodules:
        rsu[1]: RSU {
            @display("p=100,100");
        }
        car[10]: Car {
            @display("p=200+100*i,200");
        }
        attacker: DDoSAttacker {
            @display("p=50,50");
        }
    connections allowunconnected:
        // Define your connections here
               for i=0..sizeof(car)-1 {
            attacker.out[i] --> car[i].in++;
        }
}