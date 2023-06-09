#!/usr/bin/env python3
import gi

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk
import os

#import gtk

global file_path

class Handler:
    
    def getText(self, textview):
        buffer = textview.get_buffer()
        startIter, endIter = buffer.get_bounds()    
        text = buffer.get_text(startIter, endIter, False) 
        return text

    def on_window_main_delete_event(self, *args):
        Gtk.main_quit()

    def on_menu_quit_activate(self, *args):
        Gtk.main_quit()

    def on_menu_open_activate(self, *args):
        dialog = Gtk.FileChooserDialog("Please choose a file", window,
            Gtk.FileChooserAction.OPEN,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
             Gtk.STOCK_OPEN, Gtk.ResponseType.OK))

        #window.add_filters(dialog)

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            file_path = dialog.get_filename()
            print("Open clicked")
            print("File selected: " + file_path)
            image.set_from_file(file_path)
            #self.labelframe.set_label(os.path.basename(file_path))
            #self.editor.open_file(file_path)
        elif response == Gtk.ResponseType.CANCEL:
            print("Cancel clicked")

        dialog.destroy()

        print(response)
        
    def on_MPIfilter_activate(self, *args):
        MPIdialog.show_all()

    def on_button_aceptar_clicked(self, *args):
        MPIdialog.hide()
        head_tail = os.path.split(file_path)
        np = self.getText(number_of_processors)
        subprocess.run("mpiexec -n"+np+" ./try "+head_tail[1], shell=True)
        image.set_from_file(head_tail[0]+"flor.png")
        
    def on_button_cancel_clicked(self, *args):
        MPIdialog.hide()
    
        
        #dlg = gtk.FileChooserDialog("Open..", None, gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        #response = dlg.run()
        #self.text.set_text(dlg.get_filename())
        #dlg.destroy()

    
#    def onButtonPressed(self, button):
#        print("Hello World!")

file_path = ""
builder = Gtk.Builder()
builder.add_from_file("readpng.glade")

builder.connect_signals(Handler())

window = builder.get_object("window_main")
image = builder.get_object("GtkImage")
MPIdialog = builder.get_object("MPIdialog")
number_of_processors = builder.get_object("number_of_processors")
#window_selector = builder.get_object("filechooserdialog")

window.show_all()

Gtk.main()
