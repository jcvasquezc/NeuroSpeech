# -*- coding: utf-8 -*-
"""
Created on Sat Sep 10 13:55:53 2016

@author: elmon
"""

import numpy as np
import matplotlib.pyplot as plt

# Taken from https://www.kaggle.com/wenxuanchen/d/abcsds/pokemon/pokemon-visualization-radar-chart-t-sne

TYPE_LIST = ['Reference High', 'Reference Low', 'Patient']

COLOR_LIST = ['#F95643', '#FFFFFF', '#53AFFE']

# The colors are copied from this script: https://www.kaggle.com/ndrewgele/d/abcsds/pokemon/visualizing-pok-mon-stats-with-seaborn
# The colors look reasonable in this map: For example, Green for Grass, Red for Fire, Blue for Water...
COLOR_MAP = dict(zip(TYPE_LIST, COLOR_LIST))


# A radar chart example: http://datascience.stackexchange.com/questions/6084/how-do-i-create-a-complex-radar-chart
def _scale_data(data, ranges):
    (x1, x2), d = ranges[0], data[0]
    return [(d - y1) / (y2 - y1) * (x2 - x1) + x1 for d, (y1, y2) in zip(data, ranges)]

class RaderChart():
    def __init__(self, fig, variables, ranges, n_ordinate_levels = 6):
        angles = np.arange(0, 360, 360./len(variables))

        axes = [fig.add_axes([0.1,0.1,0.77,0.77],polar = True, label = "axes{}".format(i)) for i in range(len(variables))]
        _, text = axes[0].set_thetagrids(angles, labels = variables, fontsize=20)
        
        #for txt, angle in zip(text, angles):
            #txt.set_rotation(angle - 90)
        
        for ax in axes[1:]:
            ax.patch.set_visible(False)
            ax.xaxis.set_visible(False)
            ax.grid('off')
        
        for i, ax in enumerate(axes):
            grid = np.linspace(*ranges[i], num = n_ordinate_levels)
            
            grid_label = ['']+[str(int(x)) for x in grid[1:]]
            ax.set_rgrids(grid, labels = [], angle = angles[i])
            ax.set_ylim(*ranges[i])
        
        self.angle = np.deg2rad(np.r_[angles, angles[0]])
        self.ranges = ranges
        self.ax = axes[0]

    def plot(self, data, *args, **kw):
        sdata = _scale_data(data, self.ranges)
        self.ax.plot(self.angle, np.r_[sdata, sdata[0]], *args, **kw)

    def fill(self, data, *args, **kw):
        sdata = _scale_data(data, self.ranges)
        self.ax.fill(self.angle, np.r_[sdata, sdata[0]], *args, **kw)

    def legend(self, *args, **kw):
        self.ax.legend(*args, **kw)

# select display colors according to Pokemon's Type 1
def select_color(types):
    colors = [None] * len(types)
    used_colors = set()
    for i, t in enumerate(types):
        curr = COLOR_MAP[t]
        if curr not in used_colors:
            colors[i] = curr
            used_colors.add(curr)
    unused_colors = set(COLOR_LIST) - used_colors
    for i, c in enumerate(colors):
        if not c:
            try:
                colors[i] = unused_colors.pop()
            except:
                raise Exception('Attempt to visualize too many pokemons. No more colors available.')
    return colors




def plot_radar(df, refh, refl, use_attributes, title, namefig):
    #df = np.asarray([0.60, 4.88, 4.96, 0.40])
    #ref= np.asarray([1.20, 6.30, 7.70, 0.43])
    # In this order, 
    # HP, Defense and Sp. Def will show on left; They represent defense abilities
    # Speed, Attack and Sp. Atk will show on right; They represent attack abilities
    # Attack and Defense, Sp. Atk and Sp. Def will show on opposite positions
    #use_attributes = ['Jitter', 'Shimmer', 'APQ', 'PPQ']
    # choose the pokemons you like
    use_pokemons = ['Reference', 'Patient', 'Reference Low']
    
    #df_plot = df[df['Name'].map(lambda x:x in use_pokemons)==True]
    datas = [refh, df, refl] 

    ranges = [[2**-20, max([df[attr], refh[attr], refl[attr]])] for attr in range(len(use_attributes))]
    colorsbg = ['#53AFFE', '#8ED752', '#FFFFFF']
    colors = ['#53AFFE', '#8ED752', '#53AFFE']
    fig = plt.figure(figsize=(9, 9))
    radar = RaderChart(fig, use_attributes, ranges, len(df))
    for data, color, bg, pokemon in zip(datas, colors, colorsbg, use_pokemons):
        if bg=='#FFFFFF':
            radar.plot(data, color = color, linewidth=2.0)
            radar.fill(data, alpha = 1, color = bg)
            radar.legend(bbox_to_anchor=(1.17, 1.17), fontsize=21)
        else:
            radar.plot(data, color = color, label = pokemon, linewidth=2.0)
            radar.fill(data, alpha = 0.5, color = bg)
            radar.legend(bbox_to_anchor=(1.17, 1.17), fontsize=21)
    plt.title(title, fontsize=20)
    #plt.show()    
    plt.savefig(namefig)
