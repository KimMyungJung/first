﻿using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class DropObject : MonoBehaviour, IDropHandler, IPointerEnterHandler, IPointerExitHandler
{
    public MeshRenderer receivingRenderer;
    public Color highlightColor = Color.black;
    public string textureName = "";

    private Material rendererMat;
    private Color normalColor;
    private Texture droppedTexture;

#if UNITY_EDITOR
    private void Reset()
    {

        receivingRenderer = GetComponentInChildren<MeshRenderer>();
        Debug.Log("Drop Reset");
    }
#endif

    void Start()
    {
        
    }

    public void OnEnable()
    {

        if (receivingRenderer != null)
        {
            rendererMat = receivingRenderer.material;
            normalColor = rendererMat.color;
            receivingRenderer.sharedMaterial = rendererMat;
        }

    }

    public void OnDrop(PointerEventData data)
    {
        if (rendererMat != null)
        {
            rendererMat.color = normalColor;

            var dropSprite = GetDropSprite(data);
            if (dropSprite != null)
            {
                rendererMat.mainTexture = droppedTexture = dropSprite.texture;


                string[] spiltString = droppedTexture.name.Split('(') ;
                textureName = spiltString[0].Trim();
                //GetComponent<setParent>().childTexture;
                AudioSource audio = GameObject.Find("Room").GetComponent<AudioSource>();
                audio.clip = GameObject.Find("Room").GetComponent<SoundManager>().paperSound;
                audio.Play();
                
                //벽
                if(transform.root.name == "Room")
                {
                    Debug.Log("Room Texture");
                    GetComponent<setTexture>().SetSyncWallTexture(0, GetComponent<setTexture>().getTextureNum(textureName));
                }
                //자식 없는 객체
                else if (transform.parent.name == "Furniture")
                {
                    GetComponent<setParent>().SetSyncListInt(0, GetComponent<setParent>().getTextureNum(textureName));
                }
                //자식 있는 객체
                else
                {
                    transform.parent.GetComponent<setParent>().SetSyncListInt(transform.GetSiblingIndex(),
                        transform.parent.GetComponent<setParent>().getTextureNum(textureName));
                }  
            }
        }
    }

    public void OnPointerEnter(PointerEventData data)
    {
        Debug.Log("Pointer Enter");
        if (rendererMat != null)
        {
            var dropSprite = GetDropSprite(data);
            if (dropSprite != null)
            {
                //rendererMat.color = highlightColor;
                rendererMat.color = Color.black;
                rendererMat.mainTexture = null;
            }
        }
    }

    public void OnPointerExit(PointerEventData data)
    {
        if (rendererMat != null)
        {
            rendererMat.color = normalColor;
            rendererMat.mainTexture = droppedTexture;
        }
    }

    private Sprite GetDropSprite(PointerEventData data)
    {
        var originalObj = data.pointerDrag;
        if (originalObj == null) { return null; }

        var dragMe = originalObj.GetComponent<DragImage>();
        if (dragMe == null) { return null; }

        var srcImage = originalObj.GetComponent<Image>();
        if (srcImage == null) { return null; }

        
        return srcImage.sprite;
    
    }

    //추가한 코드
    public void SetMaterial(Texture newTexture)
    {
        //rendererMat.mainTexture = newTexture;
        droppedTexture = newTexture;
    }


}
