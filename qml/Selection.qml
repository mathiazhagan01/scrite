/****************************************************************************
**
** Copyright (C) TERIFLIX Entertainment Spaces Pvt. Ltd. Bengaluru
** Author: Prashanth N Udupa (prashanth.udupa@teriflix.com)
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

import QtQuick 2.13
import QtQuick.Controls 2.13

Item {
    id: selection

    property alias active: tightRect.visible
    property var items: []
    signal moveItem(Item item, real dx, real dy)
    signal placeItem(Item item)

    function init(givenItems, rectangle) {
        if(givenItems === undefined || givenItems === null)
            return

        var bounds = {
            "p1": { x: -1, y: -1 },
            "p2": { x: -1, y: -1 },
            "unite": function(pt) {
                if(this.p1.x < 0 || this.p1.y < 0) {
                    this.p1.x = pt.x
                    this.p1.y = pt.y
                } else {
                    this.p1.x = Math.min(this.p1.x, pt.x)
                    this.p1.y = Math.min(this.p1.y, pt.y)
                }
                if(this.p2.x < 0 || this.p2.y < 0) {
                    this.p2.x = pt.x
                    this.p2.y = pt.y
                } else {
                    this.p2.x = Math.max(this.p2.x, pt.x)
                    this.p2.y = Math.max(this.p2.y, pt.y)
                }

                this.p1.x = Math.round(this.p1.x)
                this.p2.x = Math.round(this.p2.x)
                this.p1.y = Math.round(this.p1.y)
                this.p2.y = Math.round(this.p2.y)
            }
        }

        var selectedItems = []
        var count = givenItems.count
        for(var i=0; i<count; i++) {
            var item = givenItems.itemAt(i)
            var p1 = Qt.point(item.x, item.y)
            var p2 = Qt.point(item.x+item.width, item.y+item.height)
            var areaContainsPoint = function(p) {
                return rectangle.left <= p.x && p.x <= rectangle.right &&
                        rectangle.top <= p.y && p.y <= rectangle.bottom;
            }
            if(areaContainsPoint(p1) || areaContainsPoint(p2)) {
                bounds.unite(p1)
                bounds.unite(p2)
                selectedItems.push(item)
            }
        }

        tightRect.x = bounds.p1.x - 10
        tightRect.y = bounds.p1.y - 10
        tightRect.width = (bounds.p2.x-bounds.p1.x+20)
        tightRect.height = (bounds.p2.y-bounds.p1.y+20)
        tightRect.topLeft = Qt.point(tightRect.x, tightRect.y)
        items = selectedItems
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        enabled: tightRect.visible
        onPressed: {
            var elements = selection.items
            if(elements.length > 0) {
                for(var i=0; i<elements.length; i++)
                    selection.placeItem(elements[i])
                selection.items = []
            }
            mouse.accepted = false
        }
    }

    Rectangle {
        id: tightRect
        color: app.translucent(app.palette.highlight,0.2)
        border { width: 2; color: app.palette.highlight }
        visible: parent.items.length > 0

        property point topLeft: Qt.point(0,0)

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis
            drag.minimumX: 0
            drag.minimumY: 0
            enabled: parent.visible
        }

        onXChanged: shiftElements()
        onYChanged: shiftElements()

        function shiftElements() {
            var elements = selection.items
            var i, item
            var dx = x - topLeft.x
            var dy = y - topLeft.y
            topLeft = Qt.point(x,y)
            for(i=0; i<elements.length; i++)
                selection.moveItem(elements[i], dx, dy)
        }
    }
}