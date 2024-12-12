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

package simulation.RSUmessage;

import org.car2x.veins.modules.mobility.traci.TraCIMobility;
import org.car2x.veins.modules.mac.ieee80211p.Mac1609_4;
import org.car2x.veins.modules.phy.PhyLayer80211p;
import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

network RSUExampleScenario extends Scenario
{
    @display("bgb=889,469");
    submodules:
        rsu[1]: RSU {
            @display("p=70,274;i=veins/sign/yellowdiamond;is=l");
        }
}

//network RSUExampleScenario {S
//    submodules:
  //      rsu[1]: RSU {
 //           @display("p=2000,2000");
 //           mobility.x = 2000;
 //           mobility.y = 2000;
 //           mobility.z = 3;
 //           nic.phy80211p.antennaOffsetZ = 0;
  //          applType = default("veins_rsu.RSUMessageApp");
  //      }
   //     car[10]: Car {
  //          @display("i=block/routing");
   //         numAttackPackets = 1000;
   //         attackInterval = 0.1s;
   //     }
//}