$fn = 64;

test = false;

hoogte  = 30;
lengte  = 83;
breedte = 60;
dekselhoogte = 2.5;

if (test) {
  translate([24, 4, 2]) lolin();
  translate([3,  lengte - 43, 4]) mosfetpcb();
  translate([14, lengte - 69, 2]) dc2dc();
  translate([4, 36, 2]) rotate([0, 0, -90]) lolin();
  translate([0, 0, hoogte]) deksel();
}

difference() {
  roundedBox(5, 1.25, lengte, breedte, hoogte);
  binnenkant(hoogte, true);
  translate([breedte - 10, 3, 8]) cube([20, 8, 12]); //usb
  translate([3, -1.25, 8]) cube([8, 20, 12]); //usb 2
  translate([breedte - 6, 15, hoogte / 2]) rotate([90, 0, 90]) dcplug(0.65);
  translate([-6, lengte - 25, hoogte / 2]) rotate([90, 0, 90]) ledlampplug();
}

translate([-breedte - 10, lengte, dekselhoogte]) rotate([180, 0, 0]) deksel();

translate([breedte - 7, lengte - 48, 0]) cube([5, 5, hoogte - 2]);
lolinhouder();
translate([0, 60, 0]) rotate([0, 0, -90]) lolinhouder();


module deksel() {
  color("lime") {
    difference() {
      union() {
        roundedBox(5, 1.25, lengte, breedte, dekselhoogte);
        translate ([0, 0, -dekselhoogte*1.5]) binnenkant(dekselhoogte, false);
      }
      translate([5, 10, -1]) knoppen(4);
    }
  }
  //translate([5, 10, -1]) knoppen(4);
}

module lolinhouder() {
  translate([20, 1, 1])
    difference() {
      cube([39, 7, hoogte - 3]);
      translate([9, 1, 0]) cube([25, 7, 7]);
      translate([9, 1, 6]) cube([35, 7, 30]);
      translate([4, 1, 0]) cube([35, 3, 30]);
    }
}

module binnenkant(hoogte, tekenBodem) {
  difference() {
    translate([2, 2, 2]) cube([56, lengte - 4, hoogte]);
    if (tekenBodem) {
      bodem();
    } else {
      translate([4, 4, -1]) cube([52, lengte - 8, hoogte * 4]);
    }
  }
}

module bodem() {
  translate([19, lengte - 8, 0]) cylinder(r = 5, h = 4);
  translate([39, lengte - 8, 0]) cylinder(r = 5, h = 4);
  hull() {
    translate([19, lengte -35, 0]) cylinder(r = 5, h = 4);
    translate([24, lengte - 35, 0]) cylinder(r = 5, h = 4);
  }
  translate([50, lengte - 38, 0]) cylinder(r = 2.5, h = 4);
}

module lolin() {
  color("blue") cube([34, 1, 25]);
  color("grey") translate([27, 1, 8]) cube([7, 3, 8]);
  color("lime") translate([15, 0, 0]) cube([12, 5, 3]);
}

module mosfetpcb() {
  color("cyan") cube([53, 40, 2]);
}

module dc2dc() {
  color("darkblue") cube([43, 21, 2]);
}

module dcplug(schroefrad) {
  translate([17.68 / 2, 1, 1]) cylinder(r =          5, h = 10);
  translate([        0, 1, 1]) cylinder(r = schroefrad, h = 10);
  translate([17.68    , 1, 1]) cylinder(r = schroefrad, h = 10);
}

module ledlampplug() {
  cylinder(r = 3, h = breedte + 20);
}

module knoppen(aantal) {
  for (index = [1 : aantal]) {
    translate([10 * index, 0, 0]) cylinder(d = 4, h = dekselhoogte+1.25);
    translate([10 * index, 0, dekselhoogte]) cylinder(d = 6, h = dekselhoogte);
  }
/*  translate([20, 0, 0]) cylinder(d = 4, h = 20);
  translate([30, 0, 0]) cylinder(d = 4, h = 20);
  translate([40, 0, 0]) cylinder(d = 4, h = 20);*/
}

module roundedBox(radius, shift, length, width, height) {
  hull() {
    translate([radius / shift,                  radius / shift, 0]) cylinder(r = radius, h = height);
    translate([radius / shift,         length - radius / shift, 0]) cylinder(r = radius, h = height);
    translate([width - radius / shift, length - radius / shift, 0]) cylinder(r = radius, h = height);
    translate([width - radius / shift,          radius / shift, 0]) cylinder(r = radius, h = height);
  }
}

