import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

print("Chargement des données de simulation...")
# Lecture du fichier txt. Les colonnes sont séparées par des espaces.
df = pd.read_csv("simulation.txt", sep=" ", names=["time", "id", "type", "x", "y"])

# Récupération des différents instants sauvegardés (les frames)
times = df['time'].unique()
print(f"{len(times)} frames trouvées. Préparation de l'animation...")

# Création de la figure
fig, ax = plt.subplots(figsize=(10, 6))

# Dimensions de la boîte (à adapter selon ton domaine spatial L1, L2)
ax.set_xlim(40, 210)  # On centre sur l'action (le bloc fait env 160 de large centré sur 125)
ax.set_ylim(0, 100)   # On s'intéresse surtout au bas de la boîte
ax.set_aspect('equal')
ax.set_title("Collision de particules de Lennard-Jones")
ax.set_xlabel("Position X")
ax.set_ylabel("Position Y")

# Initialisation du nuage de points
scatter = ax.scatter([], [], s=5) # s gère la taille des points

# Dictionnaire pour mapper le type de particule ("Rouge" ou "Bleu") à une vraie couleur matplotlib
color_map = {'Bleu': 'blue', 'Rouge': 'red'}

def init():
    """Initialise l'animation avec un graphe vide."""
    scatter.set_offsets(np.empty((0, 2)))
    return scatter,

def update(frame_idx):
    """Met à jour les données à chaque frame."""
    t = times[frame_idx]
    
    # Filtrer les données pour le temps t actuel
    data = df[df['time'] == t]
    
    # Mise à jour des positions (x, y)
    offsets = data[['x', 'y']].values
    scatter.set_offsets(offsets)
    
    # Mise à jour des couleurs
    colors = data['type'].map(color_map).values
    scatter.set_facecolors(colors)
    
    # Mise à jour du titre
    ax.set_title(f"Collision de particules - Temps : {t:.3f} s")
    
    return scatter,

# Création de l'animation (interval=50 ms entre chaque image)
ani = animation.FuncAnimation(fig, update, frames=len(times), 
                              init_func=init, blit=True, interval=50)

plt.show()

# Décommente la ligne ci-dessous si tu veux sauvegarder l'animation en MP4 (nécessite ffmpeg)
# ani.save("collision.mp4", fps=30)