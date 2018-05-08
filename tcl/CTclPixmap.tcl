proc CTclPixmapInitVars { } {
  gset ctcl_pixmap_grid 1
  gset ctcl_keep_aspect 1
}

proc CTclPixmapCreateBitmaps { } {
  set    str "R0lGODlhEAAQAPAAAAAAAAAAACH5BAEAAAAALAAAAAAQABAAAAIOhI+py+0P"
  append str "o5y02ouzPgUAOw=="

  gset blank16_image [image create photo -data $str]

  set    str "R0lGODlhDAAMAPAAAAAAAAAAACH5BAEAAAAALAAAAAAMAAwAAAIKhI+py+0P"
  append str "o5yUFQA7"

  gset blank12_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAIihI+pwbrt"
  append str "EAtRwgomNvrN/x0NRWaUlaUXSp7O6EbdFm9OAQA7"

  gset down_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAImhI+pa+EP"
  append str "Q3JA2ioRvTafXYWUxoleE0GkaH5l57IwKmNu+jD6vhQAOw=="

  gset fliph_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAIpBGKpmMF6"
  append str "XpRzxYaku3u3U2WLSGqgQ47naWYdiK0u1NWQ8Xr3pO8VVAAAOw=="

  gset flipv_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAEALAAAAAAPAA8AAAInhB9xuJDZ"
  append str "FHqQLRqNdFuq13AftYkkOGHoBXmTVZpZujpRbd/hyT8FADs="

  gset fold_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAIihI+pEKvR"
  append str "3IFRQiZZu+5uShmeBnIaVpqnuowZ+YrVO8dKAQA7"

  gset left_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAIihI+pELvR"
  append str "HIJRMlYdxJPSuxmeB0bj1oRPmimshbUrLNJWAQA7"

  gset right_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAImhI+pEKvR"
  append str "3IFRQiZZu+5uWmkYJ06bBoZeRSWeGRqtTCLk/MCZUgAAOw=="

  gset rotatel_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAImhI+pELvR"
  append str "HIJRMlYdxIruth0hB0YedY2o8WVrmmHVK6oxq1o6UAAAOw=="

  gset rotater_image [image create photo -data $str]

  set    str "R0lGODlhDwAPAPAAAAAAAAAAACH5BAEAAAAALAAAAAAPAA8AAAIghI+pEKsB"
  append str "3YnRUdZe2zhdXmHQSIbIKE2mtS5oKsKx7BQAOw=="

  gset up_image [image create photo -data $str]

  set    str "R0lGODlhDAAMAPEAAAAAAP///wAAAAAAACH5BAEAAAIALAAAAAAMAAwAAAIT"
  append str "hIynmt28noOPUgntxEYzHnlAAQA7"

  gset stipple12_image [image create photo -data $str]
}

proc CTclPixmapCreateWidgets { parent } {
  wm title $parent. "CTclPixmap"

  wm protocol $parent. WM_DELETE_WINDOW CTclPixmapExitCmd

  gset ctcl_pixmap_root $parent.

  #---

  set menu_frame $parent.menu_frame

  frame $menu_frame

  set menubar $parent.menubar

  menu $menubar

  $parent. config -menu $menubar

  set file_menu [CTclAppMenuAdd $parent.menubar "File" 0]

  CTclAppMenuAddCmd "Load ..."    CTclPixmapLoadCmd   0
  CTclAppMenuAddCmd "Insert ..."  CTclPixmapInsertCmd 0
  CTclAppMenuAddCmd "Save "       CTclPixmapSaveCmd   0
  CTclAppMenuAddCmd "Save As ..." CTclPixmapSaveAsCmd 5
  CTclAppMenuAddSep
  CTclAppMenuAddCmd "Resize ..."  CTclPixmapResizeCmd  0
  CTclAppMenuAddCmd "Rescale ..." CTclPixmapRescaleCmd 3
  CTclAppMenuAddSep
  CTclAppMenuAddCmd "Exit"        CTclPixmapExitCmd 1

  set edit_menu [CTclAppMenuAdd $parent.menubar "Edit" 0]

  CTclAppMenuAddChk "Grid"      CTclPixmapGridCmd ctcl_pixmap_grid 0
  CTclAppMenuAddSep
  CTclAppMenuAddCmd "Zoom In"   CTclPixmapZoomInCmd  5
  CTclAppMenuAddCmd "Zoom Out"  CTclPixmapZoomOutCmd 5
  CTclAppMenuAddSep
  CTclAppMenuAddCmd "Text ..."  CTclPixmapTextCmd 0

  set color_menu [CTclAppMenuAdd $parent.menubar "Color" 0]

  CTclAppMenuAddCmd "Add Color ..." CTclPixmapAddColorCmd 0

  set help_menu [CTclAppMenuAdd $parent.menubar "Help" 0]

  CTclAppMenuAddCmd "Help" CTclPixmapHelp 0

  pack $menu_frame

  #---

  set main_frame $parent.main_frame

  frame $main_frame

  #---

  set functions_frame $main_frame.functions_frame

  frame $functions_frame

  #---

  button $functions_frame.undo -text "Undo" -command CTclPixmapUndoCmd

  pack $functions_frame.undo -side top -fill x

  button $functions_frame.redo -text "Redo" -command CTclPixmapRedoCmd

  pack $functions_frame.redo -side top -fill x

  #---

  button $functions_frame.clear -text "Clear" -command CTclPixmapClearCmd

  pack $functions_frame.clear -side top -fill x

  button $functions_frame.set -text "Set" -command CTclPixmapSetCmd

  pack $functions_frame.set -side top -fill x

  button $functions_frame.invert -text "Invert" -command CTclPixmapInvertCmd

  pack $functions_frame.invert -side top -fill x

  button $functions_frame.redraw -text "Redraw" -command CTclPixmapRedrawCmd

  pack $functions_frame.redraw -side top -fill x

  #---

  gset ctcl_pixmap_function point

  #---

  radiobutton $functions_frame.copy -text "Copy" \
              -value copy -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.copy -side top -anchor w

  radiobutton $functions_frame.move -text "Move" \
              -value move -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.move -side top -anchor w

  radiobutton $functions_frame.mark -text "Mark" \
              -value mark -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.mark -side top -anchor w

  #---

  button $functions_frame.unmark -text "Unmark" \
         -command CTclPixmapUnmarkCmd

  pack $functions_frame.unmark -side top -fill x

  #---

  set bitmaps_frame $functions_frame.bitmaps_frame

  frame $bitmaps_frame

  global fliph_image flipv_image fold_image rotatel_image rotater_image
  global up_image left_image right_image down_image

  button $bitmaps_frame.flip_x   -image $fliph_image \
         -command CTclPixmapFlipHCmd
  button $bitmaps_frame.scroll_u -image $up_image \
         -command CTclPixmapUpCmd
  button $bitmaps_frame.flip_y   -image $flipv_image \
         -command CTclPixmapFlipVCmd
  button $bitmaps_frame.scroll_l -image $left_image \
         -command CTclPixmapLeftCmd
  button $bitmaps_frame.flip_xy  -image $fold_image \
         -command CTclPixmapFoldCmd
  button $bitmaps_frame.scroll_r -image $right_image \
         -command CTclPixmapRightCmd
  button $bitmaps_frame.rotate_a -image $rotatel_image \
         -command CTclPixmapRotateLCmd
  button $bitmaps_frame.scroll_d -image $down_image \
         -command CTclPixmapDownCmd
  button $bitmaps_frame.rotate_c -image $rotater_image \
         -command CTclPixmapRotateRCmd

  grid $bitmaps_frame.flip_x   -column 0 -row 0 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.scroll_u -column 1 -row 0 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.flip_y   -column 2 -row 0 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.scroll_l -column 0 -row 1 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.flip_xy  -column 1 -row 1 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.scroll_r -column 2 -row 1 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.rotate_a -column 0 -row 2 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.scroll_d -column 1 -row 2 -sticky w -padx 4 -pady 4
  grid $bitmaps_frame.rotate_c -column 2 -row 2 -sticky w -padx 4 -pady 4

  pack $bitmaps_frame -side top

  #---

  radiobutton $functions_frame.point -text "Point" \
              -value point -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.point -side top -anchor w

  radiobutton $functions_frame.curve -text "Curve" \
              -value curve -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.curve -side top -anchor w

  radiobutton $functions_frame.line -text "Line" \
              -value line -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.line -side top -anchor w

  radiobutton $functions_frame.rectangle -text "Rectangle" \
              -value rectangle -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.rectangle -side top -anchor w

  radiobutton $functions_frame.filled_rectangle -text "Filled Rectangle" \
              -value filled_rectangle -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.filled_rectangle -side top -anchor w

  radiobutton $functions_frame.circle -text "Circle" \
              -value circle -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.circle -side top -anchor w

  radiobutton $functions_frame.filled_circle -text "Filled Circle" \
              -value filled_circle -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.filled_circle -side top -anchor w

  radiobutton $functions_frame.flood_fill -text "Flood Fill" \
              -value flood_fill -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.flood_fill -side top -anchor w

  radiobutton $functions_frame.text -text "Text" \
              -value text -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.text -side top -anchor w

  radiobutton $functions_frame.set_hot_spot -text "Set Hot Spot" \
              -value set_hot_spot -variable ctcl_pixmap_function \
              -command CTclPixmapSetFunctionCmd

  pack $functions_frame.set_hot_spot -side top -anchor w

  #---

  button $functions_frame.clear_hot_spot -text "Clear Hot Spot" \
         -command CTclPixmapClearHotSpotCmd

  pack $functions_frame.clear_hot_spot -side top -anchor w

  #---

  pack $functions_frame -side left -fill y

  #---

  set canvas_pane $main_frame.canvas_pane

  tixPanedWindow $canvas_pane -orientation vertical

  $canvas_pane add top -size 100
  $canvas_pane add bot

  set top_frame $canvas_pane.top
  set bot_frame $canvas_pane.bot

  #----

  global blank12_image stipple12_image

  #----

  set fg_bg_frame $top_frame.fg_bg_frame

  frame $fg_bg_frame -borderwidth 2 -relief sunken

  #--

  set fg_frame $fg_bg_frame.fg_frame

  frame $fg_frame

  label  $fg_frame.label  -text "Fg:"
  button $fg_frame.button -image $blank12_image -bg black -fg black

  pack $fg_frame.label $fg_frame.button -side left -anchor w

  pack $fg_frame -side top -fill y

  gset ctcl_pixmap_fg_color_button $fg_frame.button

  #--

  set bg_frame $fg_bg_frame.bg_frame

  frame $bg_frame

  label  $bg_frame.label  -text "Bg:"
  button $bg_frame.button -image $blank12_image -bg white -fg white

  pack $bg_frame.label $bg_frame.button -side left -anchor w

  pack $bg_frame -side top -fill y

  gset ctcl_pixmap_bg_color_button $bg_frame.button

  #--

  pack $fg_bg_frame -side left -anchor n -fill y

  #----

  gset ctcl_pixmap_color_frame $top_frame.color_frame

  CTclPixmapClearColors

  #----

  set canvas_frame $top_frame.canvas_frame

  frame $canvas_frame -relief sunken -borderwidth 2

  CTclPixmapThumbnailCanvas $canvas_frame.thumbnail_canvas -width 64 -height 64

  pack $canvas_frame.thumbnail_canvas -side left -anchor w

  pack $canvas_frame -side right -padx 8

  #----

  set canvas_frame $bot_frame.canvas_frame

  frame $canvas_frame

  CTclPixmapCanvas $canvas_frame.canvas -width 500 -height 500

  bind $canvas_frame.canvas <Enter> {focus %W}

  scrollbar $canvas_frame.hscroll \
            -command [list $canvas_frame.canvas xview] \
            -orient horizontal
  scrollbar $canvas_frame.vscroll \
            -command [list $canvas_frame.canvas yview] \
            -orient vertical

  grid rowconfigure    $canvas_frame 0 -weight 1
  grid columnconfigure $canvas_frame 0 -weight 1

  grid $canvas_frame.canvas  -column 0 -row 0 -sticky nsew
  grid $canvas_frame.hscroll -column 0 -row 1 -sticky ew
  grid $canvas_frame.vscroll -column 1 -row 0 -sticky ns

  pack $canvas_frame -side top -fill both -expand true -anchor nw

  gset ctcl_pixmap_canvas  $canvas_frame.canvas
  gset ctcl_pixmap_hscroll $canvas_frame.hscroll
  gset ctcl_pixmap_vscroll $canvas_frame.vscroll

  #----

  pack $canvas_pane -side left -fill both -expand true

  #----

  pack $main_frame -side top -fill both -expand true

  #----

  set status_frame $parent.status_frame

  frame $status_frame -relief ridge -borderwidth 1

  label $status_frame.label -text " "

  pack $status_frame.label -side left -fill x

  pack $status_frame -side top -fill x

  gset ctcl_pixmap_status_frame $status_frame
}

proc CTclPixmapClearColors { } {
  global ctcl_pixmap_color_frame
  global ctcl_pixmap_color_num

  if {[winfo exists $ctcl_pixmap_color_frame]} {
    destroy $ctcl_pixmap_color_frame
  }

  frame $ctcl_pixmap_color_frame -borderwidth 2 -relief sunken

  set ctcl_pixmap_color_num 0

  pack $ctcl_pixmap_color_frame -side left -fill both -anchor n
}

proc CTclPixmapAddColor { color id } {
  global ctcl_pixmap_color_frame
  global ctcl_pixmap_color_num
  global blank12_image stipple12_image

  set w $ctcl_pixmap_color_frame.color_$ctcl_pixmap_color_num

  if {"$color" != "none"} {
    button $w -image $blank12_image -bg $color -fg $color \
           -command [list CTclPixmapSetFgCmd $id]
  } \
  else {
    button $w -image $stipple12_image -bg white -fg black \
           -command [list CTclPixmapSetFgCmd $id]
  }

  bind $w <Button-3> [list CTclPixmapSetBgCmd $id]

  global ctcl_pixmap_color_id

  set ctcl_pixmap_color_id($w) $id

  set col [expr $ctcl_pixmap_color_num % 16]
  set row [expr $ctcl_pixmap_color_num / 16]

  grid $w -column $col -row $row -sticky n -padx 1 -pady 1

  incr ctcl_pixmap_color_num
}

proc CTclPixmapSetFgCmd { id } {
  CTclPixmapSetFgColorNum $id
}

proc CTclPixmapSetFgColor { color } {
  global ctcl_pixmap_fg_color_button
  global blank12_image stipple12_image

  if {"$color" != "none"} {
    $ctcl_pixmap_fg_color_button config \
      -image $blank12_image -bg $color -fg $color
  } \
  else {
    $ctcl_pixmap_fg_color_button config \
      -image $stipple12_image -bg white -fg black
  }
}

proc CTclPixmapSetBgCmd { id } {
  CTclPixmapSetBgColorNum $id
}

proc CTclPixmapSetBgColor { color } {
  global ctcl_pixmap_bg_color_button
  global blank12_image stipple12_image

  if {"$color" != "none"} {
    $ctcl_pixmap_bg_color_button config \
      -image $blank12_image -bg $color -fg $color
  } \
  else {
    $ctcl_pixmap_bg_color_button config \
      -image $stipple12_image -bg white -fg black
  }
}

proc CTclPixmapSetTitle { title } {
  global ctcl_pixmap_root

  wm title $ctcl_pixmap_root $title
}

proc CTclPixmapSetStatus { filename width height } {
  set str "Filename: $filename,  Size: $width x $height"

  global ctcl_pixmap_status_frame

  $ctcl_pixmap_status_frame.label config -text $str

  gset ctcl_pixmap_width  $width
  gset ctcl_pixmap_height $height
}

proc CTclPixmapCanvasSetXScrollPercent { start end } {
  global ctcl_pixmap_hscroll

  $ctcl_pixmap_hscroll set $start $end
}

proc CTclPixmapCanvasSetYScrollPercent { start end } {
  global ctcl_pixmap_vscroll

  $ctcl_pixmap_vscroll set $start $end
}

proc CTclPixmapLoadCmd { } {
  if {! [winfo exists .load_filesel]} {
    tixExFileSelectDialog .load_filesel -command CTclPixmapLoadOKCmd
  }

  .load_filesel popup
}

proc CTclPixmapLoadOKCmd { filename } {
  CTclPixmapLoadFile $filename
}

proc CTclPixmapInsertCmd { } {
  if {! [winfo exists .insert_filesel]} {
    tixExFileSelectDialog .insert_filesel -command CTclPixmapInsertOKCmd
  }

  .insert_filesel popup
}

proc CTclPixmapInsertOKCmd { filename } {
  CTclPixmapInsertFile $filename
}

proc CTclPixmapSaveCmd { } {
  CTclPixmapSaveFile
}

proc CTclPixmapSaveAsCmd { } {
  if {! [winfo exists .save_filesel]} {
    tixExFileSelectDialog .save_filesel -command CTclPixmapSaveOKCmd
  }

  .save_filesel popup
}

proc CTclPixmapSaveOKCmd { filename } {
  CTclPixmapSaveFile $filename
}

proc CTclPixmapResizeCmd { } {
  set dialog .resize_dialog

  gset ctcl_pixmap_resize_dialog $dialog

  toplevel $dialog

  wm title $dialog "Resize"

  wm protocol $dialog WM_DELETE_WINDOW CTclPixmapResizeCancelCmd

  set frame $dialog.frame

  frame $frame

  set width_frame $frame.width_frame

  frame $width_frame

  label $width_frame.label -text "Width" -width 8
  entry $width_frame.entry -textvariable ctcl_pixmap_width

  pack $width_frame.label $width_frame.entry -side left

  pack $width_frame -side top -fill x

  set height_frame $frame.height_frame

  frame $height_frame

  label $height_frame.label -text "Height" -width 8
  entry $height_frame.entry -textvariable ctcl_pixmap_height

  pack $height_frame.label $height_frame.entry -side left

  pack $height_frame -side top -fill x

  pack $frame -side top -expand true -fill both

  CTclHSeparator $dialog.sep1

  set frame $dialog.button_frame

  frame $frame

  button $frame.ok     -text "OK"     -command CTclPixmapResizeOKCmd
  button $frame.apply  -text "Apply"  -command CTclPixmapResizeApplyCmd
  button $frame.cancel -text "Cancel" -command CTclPixmapResizeCancelCmd

  pack $frame.ok $frame.apply $frame.cancel -side left

  pack $frame -side top -expand true -fill both
}

proc CTclPixmapResizeOKCmd { } {
  CTclPixmapResizeApplyCmd

  CTclPixmapResizeCancelCmd
}

proc CTclPixmapResizeApplyCmd { } {
  global ctcl_pixmap_width ctcl_pixmap_height

  CTclPixmapResize $ctcl_pixmap_width $ctcl_pixmap_height
}

proc CTclPixmapResizeCancelCmd { } {
  global ctcl_pixmap_resize_dialog

  destroy $ctcl_pixmap_resize_dialog
}

proc CTclPixmapRescaleCmd { } {
  global ctcl_pixmap_width ctcl_pixmap_height

  gset ctcl_pixmap_width1  $ctcl_pixmap_width
  gset ctcl_pixmap_height1 $ctcl_pixmap_height

  #---

  set dialog .rescale_dialog

  gset ctcl_pixmap_rescale_dialog $dialog

  toplevel $dialog

  wm title $dialog "Rescale"

  wm protocol $dialog WM_DELETE_WINDOW CTclPixmapRescaleCancelCmd

  set frame $dialog.frame

  frame $frame

  set width_frame $frame.width_frame

  frame $width_frame

  label $width_frame.label -text "Width" -width 8
  entry $width_frame.entry -textvariable ctcl_pixmap_width

  pack $width_frame.label $width_frame.entry -side left

  pack $width_frame -side top -fill x

  set height_frame $frame.height_frame

  frame $height_frame

  label $height_frame.label -text "Height" -width 8
  entry $height_frame.entry -textvariable ctcl_pixmap_height

  pack $height_frame.label $height_frame.entry -side left

  pack $height_frame -side top -fill x

  pack $frame -side top -expand true -fill both

  checkbutton $frame.keep_aspect -text "Keep Aspect" -variable ctcl_keep_aspect \
              -offvalue 0 -onvalue 1

  pack $frame.keep_aspect -side top -side left

  CTclHSeparator $dialog.sep1

  set frame $dialog.button_frame

  frame $frame

  button $frame.ok     -text "OK"     -command CTclPixmapRescaleOKCmd
  button $frame.apply  -text "Apply"  -command CTclPixmapRescaleApplyCmd
  button $frame.cancel -text "Cancel" -command CTclPixmapRescaleCancelCmd

  pack $frame.ok $frame.apply $frame.cancel -side left

  pack $frame -side top -expand true -fill both
}

proc CTclPixmapRescaleOKCmd { } {
  CTclPixmapRescaleApplyCmd

  CTclPixmapRescaleCancelCmd
}

proc CTclPixmapRescaleApplyCmd { } {
  global ctcl_pixmap_width1 ctcl_pixmap_height1
  global ctcl_pixmap_width ctcl_pixmap_height ctcl_keep_aspect

  if {$ctcl_keep_aspect} {
    set ws [expr {(1.0*$ctcl_pixmap_width )/$ctcl_pixmap_width1 }]
    set hs [expr {(1.0*$ctcl_pixmap_height)/$ctcl_pixmap_height1}]

puts "$ctcl_pixmap_width $ctcl_pixmap_height"
puts "$ws $hs"
    if {$ctcl_pixmap_width  > $ctcl_pixmap_width1 ||
        $ctcl_pixmap_height > $ctcl_pixmap_height1} {
      if {$ws > $hs} {
        set ctcl_pixmap_height [expr {int($ctcl_pixmap_height1*$ws)}]
      } \
      else {
        set ctcl_pixmap_width  [expr {int($ctcl_pixmap_width1 *$hs)}]
      }
    } \
    else {
      if {$ws < $hs} {
        set ctcl_pixmap_height [expr {int($ctcl_pixmap_height1*$ws)}]
      } \
      else {
        set ctcl_pixmap_width  [expr {int($ctcl_pixmap_width1 *$hs)}]
      }
    }
puts "$ctcl_pixmap_width $ctcl_pixmap_height"
  }

  CTclPixmapRescale $ctcl_pixmap_width $ctcl_pixmap_height
}

proc CTclPixmapRescaleCancelCmd { } {
  global ctcl_pixmap_rescale_dialog

  destroy $ctcl_pixmap_rescale_dialog
}

proc CTclPixmapExitCmd { } {
  CTclPixmapExit
}

proc CTclPixmapGridCmd { } {
  global ctcl_pixmap_grid

  CTclPixmapSetGrid $ctcl_pixmap_grid
}

proc CTclPixmapZoomInCmd { } {
  CTclPixmapZoom 1
}

proc CTclPixmapZoomOutCmd { } {
  CTclPixmapZoom -1
}

proc CTclPixmapTextCmd { } {
  set dialog .text_dialog

  gset ctcl_pixmap_text_dialog $dialog

  toplevel $dialog

  wm title $dialog "Set Text Preferences" 

  wm protocol $dialog WM_DELETE_WINDOW CTclPixmapTextCancelCmd

  #---

  set frame $dialog.name_frame

  frame $frame

  label $frame.label -text "Name" -width 6 -anchor w
  entry $frame.entry -textvariable ctcl_pixmap_font_name

  pack $frame.label $frame.entry -side left

  pack $frame -side top -expand true -fill both

  #---

  set frame $dialog.style_frame

  frame $frame

  label         $frame.label -text "Style" -width 6 -anchor w
  tixOptionMenu $frame.entry -variable ctcl_pixmap_font_style \
                -dynamicgeometry true

  foreach item { Normal Bold Italic } {
    $frame.entry add command $item
  }

  pack $frame.label $frame.entry -side left

  pack $frame -side top -expand true -fill both

  #---

  set frame $dialog.size_frame

  frame $frame

  label $frame.label -text "Size" -width 6 -anchor w
  entry $frame.entry -textvariable ctcl_pixmap_font_size

  pack $frame.label $frame.entry -side left

  pack $frame -side top -expand true -fill both

  #---

  set frame $dialog.angle_frame

  frame $frame

  label $frame.label -text "Angle" -width 6 -anchor w
  entry $frame.entry -textvariable ctcl_pixmap_font_angle

  pack $frame.label $frame.entry -side left

  pack $frame -side top -expand true -fill both

  #---

  set frame $dialog.text_frame

  frame $frame

  label $frame.label -text "Text" -width 6 -anchor w
  entry $frame.entry -textvariable ctcl_pixmap_font_text

  pack $frame.label $frame.entry -side left

  pack $frame -side top -expand true -fill both

  #---

  CTclHSeparator $dialog.sep1

  #---

  set frame $dialog.button_frame

  frame $frame

  button $frame.ok     -text "OK"     -command CTclPixmapTextOKCmd
  button $frame.apply  -text "Apply"  -command CTclPixmapTextApplyCmd
  button $frame.cancel -text "Cancel" -command CTclPixmapTextCancelCmd

  pack $frame.ok $frame.apply $frame.cancel -side left

  pack $frame -side top -expand true -fill both
}

proc CTclPixmapTextOKCmd { } {
  CTclPixmapTextApplyCmd

  CTclPixmapTextCancelCmd
}

proc CTclPixmapTextApplyCmd { } {
  global ctcl_pixmap_font_name
  global ctcl_pixmap_font_style
  global ctcl_pixmap_font_size
  global ctcl_pixmap_font_angle
  global ctcl_pixmap_font_text

  CTclPixmapSetTextFont $ctcl_pixmap_font_name \
                        $ctcl_pixmap_font_style \
                        $ctcl_pixmap_font_size \
                        $ctcl_pixmap_font_angle
  CTclPixmapSetTextStr  $ctcl_pixmap_font_text
}

proc CTclPixmapTextCancelCmd { } {
  global ctcl_pixmap_text_dialog

  if {[winfo exists $ctcl_pixmap_text_dialog]} {
    destroy $ctcl_pixmap_text_dialog
  }
}

proc CTclPixmapAddColorCmd { } {
  set dialog .add_color_dialog

  gset ctcl_pixmap_add_color_dialog $dialog

  toplevel $dialog

  wm title $dialog "Add Color" 

  wm protocol $dialog WM_DELETE_WINDOW CTclPixmapAddColorCancelCmd

  #---

  set frame $dialog.frame

  frame $frame

  label $frame.label -text "Name"
  entry $frame.entry -textvariable ctcl_pixmap_color_name

  pack $frame.label $frame.entry -side left

  pack $frame -side top -expand true -fill both

  #---

  CTclHSeparator $dialog.sep1

  #---

  set frame $dialog.button_frame

  frame $frame

  button $frame.ok     -text "OK"     -command CTclPixmapAddColorOKCmd
  button $frame.apply  -text "Apply"  -command CTclPixmapAddColorApplyCmd
  button $frame.cancel -text "Cancel" -command CTclPixmapAddColorCancelCmd

  pack $frame.ok $frame.apply $frame.cancel -side left

  pack $frame -side top -expand true -fill both
}

proc CTclPixmapAddColorOKCmd { } {
  CTclPixmapAddColorApplyCmd

  CTclPixmapAddColorCancelCmd
}

proc CTclPixmapAddColorApplyCmd { } {
  global ctcl_pixmap_color_name

  CTclPixmapAddColorName $ctcl_pixmap_color_name
}

proc CTclPixmapAddColorCancelCmd { } {
  global ctcl_pixmap_add_color_dialog

  if {[winfo exists $ctcl_pixmap_add_color_dialog]} {
    destroy $ctcl_pixmap_add_color_dialog
  }
}

proc CTclPixmapSetFunctionCmd { } {
  global ctcl_pixmap_function

  CTclPixmapSetFunction $ctcl_pixmap_function
}

proc CTclPixmapClearHotSpotCmd { } {
  CTclPixmapClearHotSpot
}

proc CTclPixmapUnmarkCmd { } {
  CTclPixmapUnmark
}

proc CTclPixmapUndoCmd { } {
  CTclPixmapUndo
}

proc CTclPixmapRedoCmd { } {
  CTclPixmapRedo
}

proc CTclPixmapClearCmd { } {
  CTclPixmapClear
}

proc CTclPixmapSetCmd { } {
  CTclPixmapSet
}

proc CTclPixmapInvertCmd { } {
  CTclPixmapInvert
}

proc CTclPixmapRedrawCmd { } {
  CTclPixmapRedraw
}

proc CTclPixmapLeftCmd { } {
  CTclPixmapScrollX -1
}

proc CTclPixmapRightCmd { } {
  CTclPixmapScrollX 1
}

proc CTclPixmapUpCmd { } {
  CTclPixmapScrollY -1
}

proc CTclPixmapDownCmd { } {
  CTclPixmapScrollY 1
}

proc CTclPixmapFlipHCmd { } {
  CTclPixmapFlip H
}

proc CTclPixmapFlipVCmd { } {
  CTclPixmapFlip V
}

proc CTclPixmapFoldCmd { } {
  CTclPixmapFlip HV
}

proc CTclPixmapRotateLCmd { } {
  CTclPixmapRotate 90
}

proc CTclPixmapRotateRCmd { } {
  CTclPixmapRotate 270
}

CTclPixmapInitVars

CTclPixmapCreateBitmaps

CTclPixmapCreateWidgets ""
