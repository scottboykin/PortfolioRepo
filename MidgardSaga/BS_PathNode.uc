class BS_PathNode extends Actor
	notplaceable;

var NavigationPoint NavPoint;
var float GCost;
var float HCost;
var int OpenedByPathID;
var int ClosedByPathID;

function float GetFCost()
{
	return GCost + HCost;
}

DefaultProperties
{
	NavPoint = none
	GCost = 0
	HCost = 0
	OpenedByPathID = -1
	ClosedByPathID = -1
}
