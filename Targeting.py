import numpy as np
import pandas as pd
import copy
import os, shutil
from Transformation_matrices import Transformation_translation, Transformation_rotaion,camera_calibration_matrix
#from pythonProject_1.Functions import Transformation_translation



def coor_camera_to_inertial_frame (x,y,z,roll,yaw,pitch,g_roll, g_yaw, g_pitch):

#given
    #target coord in camera
    target_pix = np.array([[0],[-2],[-5],[1]]) #x_pix,y_pix, l= depth
    #distance of center of rotaion of gimbal from centroid of PA
    g_dist = np.array([0,-1,0])
    #distance of camera center vision from center of rotation of gimbal
    c_dis = np.array([0,-1,0])

    #depth (given by lidar and other method)
    #l =np.array([[-3,0,0],[0,-3,0],[0,0,-3]])

    #pixel_to_coord
    CCM_inv=camera_calibration_matrix(1,1,0,2,3)
    camera_frame = np.dot(CCM_inv, target_pix)

    #transformation_camera_gimbal
    trans_c = Transformation_translation(g_dist[0], g_dist[1], g_dist[2])
    gimbal_frame = np.dot(trans_c[1], camera_frame)


    # Transformation_gimbal_body
    rot_g = Transformation_rotaion(g_roll, g_yaw, g_pitch)
    gimbal_frame = np.dot(rot_g[0], gimbal_frame)

    trans_g = Transformation_translation(g_dist[0], g_dist[1], g_dist[2])
    body_frame= np.dot(trans_g[1], gimbal_frame)

    #Transformation_body_vehicule
    rot_v = Transformation_rotaion(roll,yaw,pitch)

    vehicule_frame = np.dot(rot_v[0],body_frame)




    #Transformation_vehicule_inertial
    trans_i = Transformation_translation(x, y, z)
    inertial_frame= np.dot(trans_i[0], vehicule_frame)

    #either use trans_i[0] with GPS coordinate of PA or use trans_i[1] with distance of inertial point from vehicule negative GPS coordinate (-x,-y,-z)

#results
    target_location  = inertial_frame[:3]
    latitude = inertial_frame[0]
    longitude = inertial_frame[1]
    height = inertial_frame[2]

    return latitude,longitude,height
if __name__ == '__main__':
    coor_camera_to_inertial_frame (x = 1,y = 1, z = 1,roll=np.pi/2,yaw=0,pitch=0,g_roll=0, g_yaw=0, g_pitch=0)
#xyz = longitude,latitude and height of PA
#g_a_dist = distance in x y z from the centroi of PA to gimbal
#g_roll, g_yaw, g_pitch = gimbal's rotation

#roll,yaw and pitch follow right hand rule convention:
#pitch up positive
#yaw counterclockwise positive
#roll to the right positive

#angles in rad