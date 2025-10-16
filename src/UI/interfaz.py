# ide_tk.py
import tkinter as tk
from tkinter import ttk, font, filedialog
from PIL import Image, ImageTk
import io
import os
import cairosvg
import subprocess
import csv
import tempfile

# Rutas relativas base (ajusta si mueves archivos)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
SYMBOLS_CSV_PATH = os.path.normpath(os.path.join(BASE_DIR, "..", "..", "tabla_simbolos.txt"))
ERRORS_TXT_PATH  = os.path.normpath(os.path.join(BASE_DIR, "..", "..", "tabla_errores.txt"))
AST_SVG_PATH     = os.path.normpath(os.path.join(BASE_DIR, "..", "..", "ast.svg"))

# -------- utilidades --------
def scrolled_text(parent, **opts):
    """Crea un Text con scrollbar vertical."""
    frame = ttk.Frame(parent)
    yscroll = ttk.Scrollbar(frame, orient="vertical")
    txt = tk.Text(frame, wrap="word", yscrollcommand=yscroll.set, **opts)
    yscroll.config(command=txt.yview)
    yscroll.pack(side="right", fill="y")
    txt.pack(side="left", fill="both", expand=True)
    return frame, txt


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Compilador")
        self.geometry("1100x700")

        # fuente monoespaciada para editor/consola
        self.mono = font.nametofont("TkFixedFont")

        # ---- barra superior ----
        toolbar = ttk.Frame(self, padding=(6, 6))
        toolbar.pack(side="top", fill="x")

        # Botón que EJECUTA el compilador con archivo temporal
        btn_run = ttk.Button(toolbar, text="Ejecutar",
                             command=lambda: self.run_compiler_with_temp("./src/compilador"))
        btn_run.pack(side="left")

        btn_open = ttk.Button(toolbar, text="Abrir .usl", command=self.on_open_file)
        btn_open.pack(side="left", padx=(6, 0))

        # Botón opcional: solo copia a consola (sin ejecutar)
        btn_copy = ttk.Button(toolbar, text="Copiar a consola", command=self.on_copy_clicked)
        btn_copy.pack(side="left", padx=(6, 0))

        # NUEVO: Botón limpiar consola
        btn_clear = ttk.Button(toolbar, text="Limpiar consola",
                               command=lambda: self.console_clear(insert_ready=True))
        btn_clear.pack(side="left", padx=(6, 0))

        # (Opcional) Atajo Ctrl+L para limpiar consola
        self.bind_all("<Control-l>", lambda e: self.console_clear(insert_ready=True))

        # ---- paned vertical: arriba (contenido), abajo (consola) ----
        vpaned = ttk.PanedWindow(self, orient="vertical")
        vpaned.pack(side="top", fill="both", expand=True)

        # ---- paned horizontal dentro de arriba: izq editor, der notebook ----
        top_pane = ttk.PanedWindow(vpaned, orient="horizontal")
        vpaned.add(top_pane, weight=4)

        # editor
        editor_frame, self.editor = scrolled_text(top_pane, font=self.mono, undo=True)
        self.editor.insert("1.0", "")
        top_pane.add(editor_frame, weight=3)

        # notebook (AST / Símbolos / Errores)
        notebook = ttk.Notebook(top_pane)
        self.notebook = notebook  # guardar referencia
        top_pane.add(notebook, weight=2)

        # --- AST (IMAGEN SVG) ---
        self.ast_svg_path = AST_SVG_PATH

        ast_frame = ttk.Frame(notebook)
        # Label que contendrá la imagen renderizada
        self.ast_image_label = tk.Label(ast_frame, bg="#1a1a1a")
        self.ast_image_label.pack(fill="both", expand=True)

        # Redibujar al cambiar tamaño de la pestaña
        ast_frame.bind("<Configure>", self._update_ast_image)
        # Primer render tras crear la UI
        self.after(100, self._update_ast_image)

        notebook.add(ast_frame, text="AST")

        # --- Símbolos (tabla) ---
        # Columnas del CSV: ID,Tipo,Entorno,Valor
        symbols_frame = ttk.Frame(notebook)
        self.symbols_tab = symbols_frame  # para detectar selección de pestaña

        self.symbols = ttk.Treeview(
            symbols_frame,
            columns=("id", "tipo", "entorno", "valor"),
            show="headings",
            height=12,
        )
        headers = (
            ("id", "ID", 150),
            ("tipo", "Tipo", 140),
            ("entorno", "Entorno", 120),
            ("valor", "Valor", 220),
        )
        for col, txt, w in headers:
            self.symbols.heading(col, text=txt)
            self.symbols.column(col, width=w, anchor="w")
        self.symbols.pack(side="left", fill="both", expand=True)
        sb_sym = ttk.Scrollbar(symbols_frame, orient="vertical", command=self.symbols.yview)
        self.symbols.configure(yscrollcommand=sb_sym.set)
        sb_sym.pack(side="right", fill="y")

        notebook.add(symbols_frame, text="Símbolos")

        # --- Errores (tabla: UNA SOLA COLUMNA 'error') ---
        errors_frame = ttk.Frame(notebook)
        self.errors_tab = errors_frame

        self.errors = ttk.Treeview(
            errors_frame,
            columns=("error",),
            show="headings",
            height=12,
        )
        self.errors.heading("error", text="Error")
        self.errors.column("error", width=600, anchor="w")
        self.errors.pack(side="left", fill="both", expand=True)
        sb_err = ttk.Scrollbar(errors_frame, orient="vertical", command=self.errors.yview)
        self.errors.configure(yscrollcommand=sb_err.set)
        sb_err.pack(side="right", fill="y")

        notebook.add(errors_frame, text="Errores")

        # Consola (abajo)
        console_frame, self.console = scrolled_text(vpaned, font=self.mono, state="normal", height=8)
        self.console.config(bg="#111", fg="#ddd", insertbackground="#ddd")
        self.console_insert("[Listo] Consola inicializada.")
        vpaned.add(console_frame, weight=1)

        # posicion inicial de divisores (opcional)
        self.after(50, lambda: vpaned.sashpos(0, int(self.winfo_height()*0.7)))

        # SOLO cargar símbolos/errores al hacer clic en las pestañas
        self.notebook.bind("<<NotebookTabChanged>>", self._on_tab_changed)

    # ---- helpers de AST/SVG ----
    def _update_ast_image(self, *_):
        """Renderiza el SVG al tamaño actual del label."""
        # Medidas disponibles
        w, h = self.ast_image_label.winfo_width(), self.ast_image_label.winfo_height()
        if w <= 1 or h <= 1:
            self.after(50, self._update_ast_image)
            return
        try:
            with open(self.ast_svg_path, "rb") as f:
                svg_bytes = f.read()
            # Render directo del SVG a PNG en memoria con el tamaño disponible
            png_bytes = cairosvg.svg2png(bytestring=svg_bytes, output_width=w, output_height=h)
            img = Image.open(io.BytesIO(png_bytes)).convert("RGBA")
            # Guardar referencia para que Tk no libere la imagen
            self._ast_imgtk = ImageTk.PhotoImage(img)
            self.ast_image_label.config(image=self._ast_imgtk, text="")
        except Exception as e:
            self.ast_image_label.config(
                text=f"No se pudo mostrar ast.svg\n{e}", fg="red", bg="#1a1a1a", anchor="center", justify="center"
            )

    # ---- helpers de consola ----
    def console_insert(self, text):
        self.console.insert("end", text + "\n")
        self.console.see("end")

    def console_clear(self, insert_ready=False):
        """Limpia la consola. Si insert_ready=True, deja un mensaje de estado."""
        self.console.delete("1.0", "end")
        self.console.see("end")
        if insert_ready:
            self.console_insert("[Listo] Consola limpia.")

    # ---- botón: SOLO copiar editor -> consola (append) ----
    def on_copy_clicked(self):
        text = self.editor.get("1.0", "end-1c")
        if not text.strip():
            self.console_insert("[Copiar] (editor vacío)")
            return
        self.console_insert("[Copiar] Contenido del editor:")
        self.console_insert(text)

    # ---- Ejecutar binario externo ./compilador sobre archivo temporal ----
    def run_compiler_with_temp(self, compiler="./src/compilador"):
        # LIMPIAR CONSOLA ANTES DE EJECUTAR
        self.console_clear()

        text = self.editor.get("1.0", "end-1c")
        if not text.strip():
            self.console_insert("[Ejecutar] (editor vacío)")
            return

        # crear archivo temporal con el código del editor
        fd, tmp_path = tempfile.mkstemp(suffix=".lang", prefix="fuente_")
        os.close(fd)
        with open(tmp_path, "w", encoding="utf-8") as f:
            f.write(text)

        cmd = [compiler, tmp_path]

        try:
            # Nota: text=True captura stdout/stderr como str (UTF-8 por defecto)
            proc = subprocess.run(cmd, capture_output=True, text=True)
            if proc.stdout:
                self.console_insert(proc.stdout.rstrip("\n"))
        except FileNotFoundError:
            self.console_insert("[Error] No se encontró el ejecutable del compilador.")
        except Exception as e:
            self.console_insert(f"[Error] {e}")
        finally:
            try:
                os.remove(tmp_path)
            except OSError:
                pass

    def on_open_file(self):
        filepath = filedialog.askopenfilename(
            title="Abrir archivo .usl",
            filetypes=[("Archivos USL", "*.usl"), ("Todos los archivos", "*.*")]
        )
        if not filepath:
            return  # Cancelado

        try:
            with open(filepath, "r", encoding="utf-8") as f:
                content = f.read()
            # Limpiar editor antes de insertar
            self.editor.delete("1.0", "end")
            self.editor.insert("1.0", content)
            self.console_insert(f"[Abrir] Archivo cargado: {os.path.basename(filepath)}")
        except Exception as e:
            self.console_insert(f"[Error al abrir archivo] {e}")

    # ---- Carga diferida de símbolos/errores (solo al abrir cada pestaña) ----
    def _on_tab_changed(self, event):
        try:
            tab_id = event.widget.select()
            current_tab = event.widget.nametowidget(tab_id)
            if current_tab is self.symbols_tab:
                self.load_symbols_from_csv(SYMBOLS_CSV_PATH)
            elif current_tab is self.errors_tab:
                self.load_errors_from_txt(ERRORS_TXT_PATH)
        except Exception as e:
            self.console_insert(f"[Notebook] Error al refrescar: {e}")

    # ---- Lectura de símbolos (CSV: ID,Tipo,Entorno,Valor) ----
    def load_symbols_from_csv(self, csv_path: str):
        # Limpiar filas actuales
        for item in self.symbols.get_children():
            self.symbols.delete(item)

        if not os.path.exists(csv_path):
            self.console_insert(f"[Símbolos] No existe: {os.path.relpath(csv_path, BASE_DIR)}")
            return

        try:
            with open(csv_path, "r", encoding="utf-8", newline="") as f:
                reader = csv.reader(f)
                rows = list(reader)
        except UnicodeDecodeError:
            with open(csv_path, "r", encoding="latin-1", newline="") as f:
                reader = csv.reader(f)
                rows = list(reader)
        except Exception as e:
            self.console_insert(f"[Símbolos] Error leyendo CSV: {e}")
            return

        # ¿Tiene encabezado?
        start_idx = 1 if rows and rows[0] and rows[0][0].strip().lower() in ("id", "nombre") else 0

        for row in rows[start_idx:]:
            if not row:
                continue
            # Esperado: ID,Tipo,Entorno,Valor
            id_ = row[0] if len(row) > 0 else ""
            tipo = row[1] if len(row) > 1 else ""
            entorno = row[2] if len(row) > 2 else ""
            valor = row[3] if len(row) > 3 else ""
            self.symbols.insert("", "end", values=(id_, tipo, entorno, valor))

    # ---- Lectura de errores (TXT: una línea = un error) ----
    def load_errors_from_txt(self, txt_path: str):
        # Limpiar filas actuales
        for item in self.errors.get_children():
            self.errors.delete(item)

        if not os.path.exists(txt_path):
            self.console_insert(f"[Errores] No existe: {os.path.relpath(txt_path, BASE_DIR)}")
            return

        try:
            with open(txt_path, "r", encoding="utf-8") as f:
                lines = f.readlines()
        except UnicodeDecodeError:
            with open(txt_path, "r", encoding="latin-1") as f:
                lines = f.readlines()
        except Exception as e:
            self.console_insert(f"[Errores] Error leyendo TXT: {e}")
            return

        for line in lines:
            msg = line.rstrip("\r\n")
            if msg:
                self.errors.insert("", "end", values=(msg,))

if __name__ == "__main__":
    App().mainloop()
