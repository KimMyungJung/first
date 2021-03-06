﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class setTexture : NetworkBehaviour
{
    private Texture[] textures;
    private int childCnt = 0;

    public SyncListInt syncListTexture = new SyncListInt();

    // Start is called before the first frame update
    void Start()
    {

        textures = GameObject.Find("Furniture").GetComponent<FurnitureManager>().FurnitureTextures;


        if (isServer)
        {
            syncListTexture.Add(-1);
        }
        else
        {
            SetTexture(gameObject, syncListTexture[0]);
        }

        syncListTexture.Callback += MyCallBack;

    }

    private void MyCallBack(SyncListInt.Operation op, int index)
    {

        Debug.Log("MyCallBack");
        int num = syncListTexture[index];

        if (num == -1) return;

        SetTexture(gameObject, num);

    }


    public void SetSyncWallTexture(int idx, int num)
    {
        Debug.Log("Set Wall Texture : " + num);

        GameObject.Find("LocalPlayer").GetComponent<isLocalPlayer>().CmdSetSyncWallTexture(gameObject, idx, num);

        //syncListTexture[idx] = num;
    }



    public int getTextureNum(string textureName)
    {
        for (int i = 0; i < textures.Length; i++)
        {
            if (string.Compare(textureName, textures[i].name) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    private void SetTexture(GameObject target, int num)
    {
        Debug.Log("SetTexture"); 
        if (num == -1) return;
        Debug.Log("Set Texture : " + num);
        target.GetComponent<MeshRenderer>().material.mainTexture = textures[num];
        target.GetComponent<DropObject>().SetMaterial(textures[num]);
        target.GetComponent<DropObject>().textureName = textures[num].name;
    }

    //친구 방에 들어갔을 때 벽지 변경
    public void EnterFriendRoom()
    {
        Debug.Log("친구방에 벽지 붙이는중..." + syncListTexture[0]);
        SetTexture(this.gameObject, syncListTexture[0]);
    }

}
