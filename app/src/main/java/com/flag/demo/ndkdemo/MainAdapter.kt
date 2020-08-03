package com.flag.demo.ndkdemo

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import androidx.recyclerview.widget.RecyclerView


class MainAdapter(val mData: List<String>, val context: Context) : RecyclerView.Adapter<MainAdapter.MainHolder>() {

    lateinit var mListener: (Int) -> Unit

    fun onButtonClickListener(listener: (Int) -> Unit) {
        this.mListener = listener
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MainHolder {
        val itemView = LayoutInflater.from(context).inflate(R.layout.item_rv_main, parent, false)
        return MainHolder(itemView)
    }

    override fun getItemCount(): Int = mData.size

    class MainHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        val btn: Button = itemView.findViewById(R.id.btn_item_main)
    }

    override fun onBindViewHolder(holder: MainHolder, position: Int) {
        holder.btn.text = mData[position]
        holder.btn.setOnClickListener {
            mListener(position)
        }
    }
}