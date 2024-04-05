package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import java.io.Serializable;
import java.util.Date;
import lombok.Data;

/**
 * 
 * @TableName amusement_facility
 */
@TableName(value ="amusement_facility")
@Data
public class AmusementFacility implements Serializable {
    /**
     * 
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 名称
     */
    private String name;

    /**
     * 介绍
     */
    private String introduction;

    /**
     * 纬度
     */
    private String longitude;

    /**
     * 经度
     */
    private String latitude;

    /**
     * 一次游玩的人数

     */
    private Integer per_user_count;

    /**
     * 预计游玩时间（以分钟为单位）
     */
    private Integer expect_time;

    /**
     * 项目类型 可多选（过山车、轨道、失重、水上、室内、旋转、鬼屋）
     */
    private String type;

    /**
     * 适合人群（成人、老少皆宜、家长监护）
     */
    private String crowd_type;

    /**
     * 设施照片
     */
    private String image_url;

    /**
     * 开放时间
     */
    private Date start_time;

    /**
     * 关闭时间
     */
    private Date close_time;

    /**
     * 状态 0-正常 1-异常（如果是在检修的时候status为1，注意夜晚闭馆未开放的时候status为0）
     */
    private Integer status;

    /**
     * 添加时间
     */
    private Date create_time;

    /**
     * 修改时间
     */
    private Date update_time;

    /**
     * 逻辑删除(0-未删除，1-已删除)
     */
    private Integer is_deleted;

    /**
     * 游玩须知
     */
    private String instruction;

    /**
     * 身高下限
     */
    private Integer height_low;

    /**
     * 身高上限
     */
    private Integer height_up;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;

    @Override
    public boolean equals(Object that) {
        if (this == that) {
            return true;
        }
        if (that == null) {
            return false;
        }
        if (getClass() != that.getClass()) {
            return false;
        }
        AmusementFacility other = (AmusementFacility) that;
        return (this.getId() == null ? other.getId() == null : this.getId().equals(other.getId()))
            && (this.getName() == null ? other.getName() == null : this.getName().equals(other.getName()))
            && (this.getIntroduction() == null ? other.getIntroduction() == null : this.getIntroduction().equals(other.getIntroduction()))
            && (this.getLongitude() == null ? other.getLongitude() == null : this.getLongitude().equals(other.getLongitude()))
            && (this.getLatitude() == null ? other.getLatitude() == null : this.getLatitude().equals(other.getLatitude()))
            && (this.getPer_user_count() == null ? other.getPer_user_count() == null : this.getPer_user_count().equals(other.getPer_user_count()))
            && (this.getExpect_time() == null ? other.getExpect_time() == null : this.getExpect_time().equals(other.getExpect_time()))
            && (this.getType() == null ? other.getType() == null : this.getType().equals(other.getType()))
            && (this.getCrowd_type() == null ? other.getCrowd_type() == null : this.getCrowd_type().equals(other.getCrowd_type()))
            && (this.getImage_url() == null ? other.getImage_url() == null : this.getImage_url().equals(other.getImage_url()))
            && (this.getStart_time() == null ? other.getStart_time() == null : this.getStart_time().equals(other.getStart_time()))
            && (this.getClose_time() == null ? other.getClose_time() == null : this.getClose_time().equals(other.getClose_time()))
            && (this.getStatus() == null ? other.getStatus() == null : this.getStatus().equals(other.getStatus()))
            && (this.getCreate_time() == null ? other.getCreate_time() == null : this.getCreate_time().equals(other.getCreate_time()))
            && (this.getUpdate_time() == null ? other.getUpdate_time() == null : this.getUpdate_time().equals(other.getUpdate_time()))
            && (this.getIs_deleted() == null ? other.getIs_deleted() == null : this.getIs_deleted().equals(other.getIs_deleted()))
            && (this.getInstruction() == null ? other.getInstruction() == null : this.getInstruction().equals(other.getInstruction()))
            && (this.getHeight_low() == null ? other.getHeight_low() == null : this.getHeight_low().equals(other.getHeight_low()))
            && (this.getHeight_up() == null ? other.getHeight_up() == null : this.getHeight_up().equals(other.getHeight_up()));
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((getId() == null) ? 0 : getId().hashCode());
        result = prime * result + ((getName() == null) ? 0 : getName().hashCode());
        result = prime * result + ((getIntroduction() == null) ? 0 : getIntroduction().hashCode());
        result = prime * result + ((getLongitude() == null) ? 0 : getLongitude().hashCode());
        result = prime * result + ((getLatitude() == null) ? 0 : getLatitude().hashCode());
        result = prime * result + ((getPer_user_count() == null) ? 0 : getPer_user_count().hashCode());
        result = prime * result + ((getExpect_time() == null) ? 0 : getExpect_time().hashCode());
        result = prime * result + ((getType() == null) ? 0 : getType().hashCode());
        result = prime * result + ((getCrowd_type() == null) ? 0 : getCrowd_type().hashCode());
        result = prime * result + ((getImage_url() == null) ? 0 : getImage_url().hashCode());
        result = prime * result + ((getStart_time() == null) ? 0 : getStart_time().hashCode());
        result = prime * result + ((getClose_time() == null) ? 0 : getClose_time().hashCode());
        result = prime * result + ((getStatus() == null) ? 0 : getStatus().hashCode());
        result = prime * result + ((getCreate_time() == null) ? 0 : getCreate_time().hashCode());
        result = prime * result + ((getUpdate_time() == null) ? 0 : getUpdate_time().hashCode());
        result = prime * result + ((getIs_deleted() == null) ? 0 : getIs_deleted().hashCode());
        result = prime * result + ((getInstruction() == null) ? 0 : getInstruction().hashCode());
        result = prime * result + ((getHeight_low() == null) ? 0 : getHeight_low().hashCode());
        result = prime * result + ((getHeight_up() == null) ? 0 : getHeight_up().hashCode());
        return result;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getClass().getSimpleName());
        sb.append(" [");
        sb.append("Hash = ").append(hashCode());
        sb.append(", id=").append(id);
        sb.append(", name=").append(name);
        sb.append(", introduction=").append(introduction);
        sb.append(", longitude=").append(longitude);
        sb.append(", latitude=").append(latitude);
        sb.append(", per_user_count=").append(per_user_count);
        sb.append(", expect_time=").append(expect_time);
        sb.append(", type=").append(type);
        sb.append(", crowd_type=").append(crowd_type);
        sb.append(", image_url=").append(image_url);
        sb.append(", start_time=").append(start_time);
        sb.append(", close_time=").append(close_time);
        sb.append(", status=").append(status);
        sb.append(", create_time=").append(create_time);
        sb.append(", update_time=").append(update_time);
        sb.append(", is_deleted=").append(is_deleted);
        sb.append(", instruction=").append(instruction);
        sb.append(", height_low=").append(height_low);
        sb.append(", height_up=").append(height_up);
        sb.append(", serialVersionUID=").append(serialVersionUID);
        sb.append("]");
        return sb.toString();
    }
}